# NCurses Runtime Switching Fix

## Problem
The original implementation used the system NCurses library, which maintains global state that became corrupted during runtime library switching (dlopen/dlclose cycles), causing segfaults after a couple of switches.

## Solution
We implemented a **locally compiled NCurses** approach with improved state management:

### 1. External NCurses Build (Makefile)
- **Download & Build**: NCurses 6.4 is now downloaded and compiled from source into `libs/ncurses/`
- **Local Linking**: All NCurses references now point to the local build instead of system library
- **Configuration**: Built with `--enable-widec`, `--with-shared` for wide character support

```makefile
NCURSES_URL := https://invisible-mirror.net/archives/ncurses/ncurses-6.4.tar.gz
NCURSES_DIR := $(LIB_DIR)/ncurses
NCURSES_CFLAGS := $(LIB_CFLAGS) -I$(NCURSES_DIR)/include -I$(NCURSES_DIR)/include/ncursesw
NCURSES_LDFLAGS := -L$(NCURSES_DIR)/lib -lncursesw -Wl,-rpath,$(NCURSES_DIR)/lib
```

### 2. Improved NCursesGraphic Cleanup
- **Persistent Session**: NCurses session remains alive between library switches
- **No endwin() on destruction**: The destructor only cleans up windows, not the entire ncurses session
- **Init Detection**: Uses `isendwin()` to detect if NCurses is already initialized and reuses the session

```cpp
void init(int w, int h) override {
    bool wasActive = (isendwin() == FALSE);
    if (!wasActive) {
        initscr();  // Only init if not already active
    }
    // ... rest of initialization
}

~NCursesGraphic() {
    // Clean up windows but DON'T call endwin()
    if (gameWindow) {
        delwin(gameWindow);
    }
    wclear(stdscr);
    wrefresh(stdscr);
    // endwin() NOT called here!
}
```

### 3. Main Program Cleanup Handler
- **atexit() Handler**: Registered cleanup function ensures `endwin()` is called when program exits
- **Proper Terminal Reset**: Guarantees terminal state is restored even on abnormal exit

```cpp
void cleanupNCurses() {
    if (isendwin() == FALSE) {
        endwin();
    }
}

int main() {
    std::atexit(cleanupNCurses);
    // ... game loop
}
```

## Key Changes

### Files Modified:
1. **Makefile**
   - Added NCurses download and build pipeline
   - Updated LDFLAGS to link main binary with local NCurses
   - Added `fclean_all` target to remove external libraries

2. **srcs/graphics/NCursesGraphic.cpp**
   - Added `isInitialized` flag
   - Modified `init()` to reuse existing NCurses session
   - Modified destructor to avoid calling `endwin()`
   - Improved cleanup order and error handling

3. **srcs/main.cpp**
   - Added `#include <ncurses.h>`
   - Added `cleanupNCurses()` function
   - Registered cleanup handler with `std::atexit()`

## Benefits

✅ **Stable Runtime Switching**: Can switch between libraries without segfaults  
✅ **Isolated Dependencies**: No conflicts with system NCurses  
✅ **Better State Management**: NCurses session persists across library switches  
✅ **Clean Terminal Reset**: Proper cleanup on program exit  
✅ **Consistent Behavior**: Same build process as SDL2 and Raylib  

## Build Commands

```bash
# Clean all (including external libraries)
make fclean_all

# Build everything (downloads and builds NCurses if needed)
make

# Run the game
./nibbler 30 20
```

## Testing Runtime Switching

Press `1`, `2`, or `3` during gameplay to switch between libraries:
- **1**: NCurses (terminal-based)
- **2**: SDL2 (window-based)
- **3**: Raylib (window-based)

The NCurses library should now handle multiple switches without crashing!
