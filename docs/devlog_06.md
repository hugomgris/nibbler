# Nibbler - Devlog - 6

## Table of Contents
1. [Day Six Plan](#61-day-six-plan)
2. [Retroceder Nunca, Rendirse Jamás](#62-a-new-foe-has-appeared)
	- [External Ncurses Build](#621-external-ncurses-build-makefile)
	- [Persistent Session Strategy](#622-persistent-session-strategy-ncursesgraphiccpp)
	- [Smart Initialization](#623-smart-initialization)
	- [Exit handler](#624-exit-handler-maincpp)
	- [Overall Sum Up](#625-overall-sum-up)
3. [Food Restocking]

<br>
<br>
<br>

## 6.1 Day Six Plan
Monday again, the sadness persists, and so does the work. I'll pick it up just where I left it last Friday: **searching for an Ncurses substitute**. Besides that, some other things that could be done today:
- Refine the food spawning
- Start menu
- Refine the ending processsing
- Start experimenting with the `3D version` (cube-based) in Raylib

<br>
<br>
<br>

## 6.2 Retroceder Nunca, Rendirse Jamás
Or, as the original title of the Van Damme classic, the infamous Bruce Lee Ghost movie, *No Retreat, No Surrender*. After the seg fault disaster which ended last Friday's work session, I came up with an idea: **What if instead of transitioning into a new library, I just switch into an external build of Ncurses and avoid the system one?**. And lo and behold: it worked.

### Understanding the Problem
The root cause of the segfault was becoming clearer with each test: **system NCurses maintains global state that gets corrupted during runtime library switching**. The flow looked like this:

1. Library is loaded (`dlopen`)
2. NCurses initialized (`initscr`)
3. NCurses cleaned up (`endwin`)
4. Library unloaded (`dlclose`)
5. **Repeat steps 1-4** ← State corruption and segfault!

The issue was particularly visible when switching back and forth between NCurses and any other library. SDL2 ↔ Raylib worked perfectly fine, but NCurses ↔ anything would crash after a couple of switches. This pointed to something specific about how NCurses handles its internal state at the system level.

<br>

### The Solution: A Three-Pronged Approach

The fix required changes in three key areas: **the build system, the NCurses graphics implementation, and the main program cleanup**. Let's break them down:

#### 6.2.1 External NCurses Build (Makefile)

Instead of using the system NCurses (which maintains problematic global state), the build now:
- Downloads NCurses 6.4 from source
- Compiles it locally in `libs/ncurses/`, just like it was already doing for SDL2 and Raylib
- Links all binaries against the local build
- Full control over library state!

The Makefile modifications were straightforward, following the same pattern already established for SDL2 and Raylib:

```makefile
# -=-=-=-=-    EXTERNAL LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

NCURSES_DIR     := $(LIB_DIR)/ncurses
NCURSES_URL     := https://invisible-mirror.net/archives/ncurses/ncurses-6.4.tar.gz
NCURSES_EXISTS  := $(wildcard $(NCURSES_DIR)/lib/libncursesw.so)

NCURSES_CFLAGS  := $(LIB_CFLAGS) -I$(NCURSES_DIR)/include -I$(NCURSES_DIR)/include/ncursesw
NCURSES_LDFLAGS := -L$(NCURSES_DIR)/lib -lncursesw -Wl,-rpath,$(NCURSES_DIR)/lib
```

The build target downloads, configures, and compiles NCurses with the necessary flags:

```makefile
ifndef NCURSES_EXISTS
	@echo "$(YELLOW)NCurses not found. Downloading and building...$(DEF_COLOR)"
	@mkdir -p $(LIB_DIR)
	@cd $(LIB_DIR) && \
		curl -L -o ncurses.tar.gz $(NCURSES_URL) && \
		tar xzf ncurses.tar.gz && \
		mv ncurses-6.4 ncurses-src && \
		cd ncurses-src && \
		./configure --prefix=$(PWD)/$(NCURSES_DIR) \
			--enable-widec \
			--with-shared \
			--without-debug \
			--enable-pc-files \
			--with-pkg-config-libdir=$(PWD)/$(NCURSES_DIR)/lib/pkgconfig && \
		make -j4 && \
		make install && \
		cd .. && \
		rm -rf ncurses-src ncurses.tar.gz
	@echo "$(GREEN)NCurses built successfully$(DEF_COLOR)"
endif
```

Key configuration flags:
- `--enable-widec`: Enables wide character support (Unicode)
- `--with-shared`: Builds shared libraries
- `--prefix`: Sets installation to our local directory

The main program also needed to link against the local NCurses for the exit handler:

```makefile
LDFLAGS := -ldl -L$(PWD)/libs/ncurses/lib -lncursesw -Wl,-rpath,$(PWD)/libs/ncurses/lib
```

<br>

#### 6.2.2 Persistent Session Strategy (NCursesGraphic.cpp)

This was the key insight: **instead of destroying the NCurses session on every library switch, keep it alive**. The destructor now only cleans up windows, not the entire session:

```cpp
~NCursesGraphic() {
    if (!isInitialized) {
        return;
    }
    
    std::cout << BBLU << "[NCurses] Cleaning up..." << RESET << std::endl;
    
    // Delete subwindow first
    if (gameWindow) {
        werase(gameWindow);
        wnoutrefresh(gameWindow);
        doupdate();
        delwin(gameWindow);
        gameWindow = nullptr;
    }
    
    // Clear main screen but DON'T call endwin()
    // This keeps ncurses alive between library switches
    if (stdscr) {
        wclear(stdscr);
        wrefresh(stdscr);
    }
    
    fflush(stdout);
    isInitialized = false;
    std::cout << BBLU << "[NCurses] Destroyed (keeping ncurses session alive)" << RESET << std::endl;
}
```

The critical change: **no `endwin()` call in the destructor**. This keeps the NCurses session persistent across library switches.

<br>

#### 6.2.3 Smart Initialization

The `init()` function needed to be smart enough to detect whether NCurses needs a fresh start or can reuse an existing session:

```cpp
void init(int w, int h) override {
    setlocale(LC_ALL, "");
    width = w;
    height = h;
    
    std::cout << BBLU << "[NCurses] Initializing: " << width << "x" << height << RESET << std::endl;
    
    // Check if ncurses was previously ended (isendwin returns TRUE if endwin was called)
    bool wasEnded = (isendwin() == TRUE);
    
    if (wasEnded || stdscr == nullptr) {
        // Initialize ncurses if it was ended or never started
        std::cout << BBLU << "[NCurses] Starting fresh ncurses session" << RESET << std::endl;
        initscr();
    } else {
        std::cout << BBLU << "[NCurses] Reusing existing ncurses session" << RESET << std::endl;
        refresh();
    }
    
    // ... rest of initialization
}
```

**Important note**: I initially got the `isendwin()` logic backwards! After testing, I discovered:
- `isendwin()` returns `0` (FALSE) when NCurses is active OR never initialized
- `isendwin()` returns `1` (TRUE) when `endwin()` has been called

This was a crucial debugging moment that cost me a bit of time but taught me to always verify library behavior with small test programs.

<br>

#### 6.2.4 Exit Handler (main.cpp)

Finally, since we're no longer calling `endwin()` during library switches, we need to ensure it gets called when the program actually exits. Enter `std::atexit()`:

```cpp
#include <ncurses.h>

// Cleanup handler for ncurses when program exits
void cleanupNCurses() {
    if (isendwin() == FALSE) {
        endwin();
        std::cout << BYEL << "[Main] Called endwin() on exit" << RESET << std::endl;
    }
}

int main(int argc, char **argv) {
    // Register cleanup handler
    std::atexit(cleanupNCurses);
    
    // ... rest of main
}
```

This guarantees that the terminal is properly reset even if the program exits abnormally.

<br>

### 6.2.5 Overall Sum-up

The new flow with a local NCurses build and persistent session:

1. Library is loaded (`dlopen`)
2. NCurses initialized ONCE (`initscr`)
3. Windows cleaned up, **session stays alive**
4. Library unloaded (`dlclose`)
5. **Repeat 1,3,4** → NCurses session persists!
6. Final cleanup on program exit (`endwin`)

By keeping the NCurses session alive and using a locally compiled version, the execution can:
- Avoid corrupting the global state
- Maintain full control over the library lifecycle
- Enable seamless switching between all three libraries

<br>
<br>
<br>

## 6.3 Food Restocking
With no more segfaults, let's tackle other pending subjects, like enhancing the repositioning of the food dot when nibbled. The main issue with the current implementation is that because it's just a brute, random index-based repositioning, there are cases in which the dot gets repositioned in an occupied cell (right now, a snake-occupied cell; in the future there might be other space-occuping stuff). To refine the repositioning process, two main ways come to mind:
- A retry based approach → Just check if the repositioning is targetting an occupied space, if so re-replace (funcional but not optimal, and once again baby Jesus would cry)
- Map the available space in the game arena so that the repositioning never targets an occupied space (refined, elegant, baby Jesus smile worthy).

So let's map. The only thing that's needed is to reserve the exact space in memory that will be needed, then go through all the cells in the game arena and check if there's a snake chunk in it. If there's not, it's available. After that, get the position from the available list, not the overall arena list. And we're golden:

```cpp
bool Food::replaceInFreeSpace(GameState *gameState)
{
	std::vector<Vec2> snakeSegments;
	for (int i = 0; i < gameState->snake->getLength(); i++) {
		snakeSegments.push_back(gameState->snake->getSegments()[i]);
	}

	std::vector<Vec2> availableCells;
	availableCells.reserve(_hLimit * _vLimit - snakeSegments.size());

	for (int y = 0; y < _vLimit; y++) {
		for (int x = 0; x < _hLimit; x++)
		{
			Vec2 candidate = {x, y};

			bool occupied = false;
			for (const auto &segment : snakeSegments) {
				if (segment.x == candidate.x && segment.y == candidate.y)
				{
					occupied = true;
					break;
				}
			}

			if (!occupied) {
				availableCells.push_back(candidate);
			}
		}
	}

	if (availableCells.empty())
	{
		std::cout << "No available cells! aka you Won, bb" << std::endl;
		return false;
	}

	int randomIndex = Utils::getRandomInt(availableCells.size());
	_position = availableCells[randomIndex];
	_foodChar = Utils::getFoodChar(Utils::getRandomInt(5));

	return true;
}
```
