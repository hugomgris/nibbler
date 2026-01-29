# Nibbler Modernization Report

## ✅ Completed Modernizations

### 1. **Library Array (main.cpp)** ✅ DONE
**Before:**
```cpp
const char *libs[] = {
    "./nibbler_ncurses.so",
    "./nibbler_sdl.so",
    "./nibbler_raylib.so"
};
```

**After:**
```cpp
constexpr std::array<std::string_view, 3> libs = {
    "./nibbler_ncurses.so",
    "./nibbler_sdl.so",
    "./nibbler_raylib.so"
};
```

**Benefits:**
- Type safety with compile-time size checking
- Zero overhead (constexpr + string_view)
- Modern C++20 standard library usage
- Iterator support for future enhancements

---

### 2. **GameState Pointers → References (DataStructs.hpp)** ✅ DONE
**Before:**
```cpp
struct GameState {
    Snake  *snake;
    Food   *food;
    const char *foodChar;  // Unused field
    // ...
};
```

**After:**
```cpp
struct GameState {
    Snake& snake;
    Food&  food;
    // Removed unused foodChar field
    // ...
};
```

**Benefits:**
- No null pointer concerns (references can't be null)
- Clearer ownership semantics (objects live in main())
- Cleaner syntax (`.` instead of `->`)
- Removed unused `foodChar` field

**Updated Files:**
- `srcs/main.cpp` - Changed initialization from `&snake, &food` to `snake, food`
- `srcs/GameManager.cpp` - Changed all `_state->snake->` to `_state->snake.`
- `srcs/Food.cpp` - Changed `gameState->snake->` to `gameState->snake.`
- `srcs/graphics/NCursesGraphic.cpp` - Changed `state.snake->` to `state.snake.`
- `srcs/graphics/SDLGraphic.cpp` - Changed `state.snake->` to `state.snake.`
- `srcs/graphics/RaylibGraphic.cpp` - Changed `state.snake` to `&state.snake`

---

## 🔍 Additional Modernization Opportunities

### 3. **Snake Fixed-Size Array → std::array**
**Current (Snake.hpp):**
```cpp
class Snake {
private:
    int   _length;
    Vec2  _segments[100];  // C-style fixed array
    // ...
};
```

**Recommendation:**
```cpp
class Snake {
private:
    int   _length;
    std::array<Vec2, 100> _segments;  // Modern fixed-size array
    // ...
};
```

**Benefits:**
- Bounds checking with `.at()`
- Iterator support
- Better integration with STL algorithms
- No behavioral change (same memory layout)

**Impact:** Low risk, minimal code changes needed

---

### 4. **enum direction → enum class Direction**
**Current (Snake.hpp):**
```cpp
enum direction {
    LEFT, RIGHT, UP, DOWN
};
```

**Recommendation:**
```cpp
enum class Direction {
    Left, Right, Up, Down
};
```

**Benefits:**
- Type safety (can't accidentally compare with integers)
- Scoped names (no global namespace pollution)
- Consistent with `GameStateType` style

**Impact:** Medium risk - need to update all usages to `Direction::Left` etc.

---

### 5. **LibraryManager Function Pointers**
**Current (LibraryManager.hpp):**
```cpp
using CreateFn = IGraphic *(*)();
using DestroyFn = void (*)(IGraphic*);
```

**Already Good!** These are appropriate for C-style function pointers from `dlsym()`.

**Note:** `std::function` would add overhead here and isn't needed.

---

### 6. **Particle System Vectors**
**Current (SDLGraphic.hpp):**
```cpp
std::vector<BorderLine> borderLines;
std::vector<Particle>   particles;
```

**Already Good!** Vectors are appropriate here for dynamic particle counts.

**Potential Enhancement:**
```cpp
// Reserve capacity upfront to avoid reallocations
particles.reserve(maxDustDensity);
```

---

### 7. **Food getFoodChar() Return Type**
**Current (Utils.cpp):**
```cpp
const char* Utils::getFoodChar(int idx) {
    switch (idx) {
        case 0: return "Ở";
        // ...
    }
}
```

**Recommendation:**
```cpp
std::string_view Utils::getFoodChar(int idx) {
    switch (idx) {
        case 0: return "Ở";
        // ...
    }
}
```

**Benefits:**
- Modern C++ string handling
- Zero-copy overhead (string_view is just pointer + length)
- Better type safety

**Impact:** Low risk, but need to verify NCurses compatibility with `std::string_view::data()`

---

### 8. **Random Number Generation Optimization**
**Current (Utils.cpp):**
```cpp
int Utils::getRandomInt(int max) {
    std::random_device rd;
    std::mt19937 gen(rd());  // Recreated every call!
    std::uniform_int_distribution<> distr(0, max);
    return distr(gen);
}
```

**Problem:** Creating `std::random_device` and `std::mt19937` on every call is expensive.

**Recommendation:**
```cpp
class Utils {
private:
    static std::mt19937& getGenerator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

public:
    static int getRandomInt(int max) {
        std::uniform_int_distribution<> distr(0, max);
        return distr(getGenerator());
    }
    // ...
};
```

**Benefits:**
- Generator initialized once (static)
- Much faster random number generation
- Thread-safe (C++11 magic statics)

**Impact:** Low risk, significant performance improvement

---

### 9. **NCurses File I/O in Rendering**
**Current (NCursesGraphic.cpp):**
```cpp
void NCursesGraphic::drawTitle(int win_height, int win_width) {
    std::ifstream file(filepath);  // Opens file every frame!
    // ...
}
```

**Problem:** File I/O during rendering (mentioned in README's "What's Left")

**Recommendation:**
- Load ASCII art files once in `init()`
- Store in `std::vector<std::string>` or `std::array<std::string, N>`
- Render from memory

**Benefits:**
- Massive performance improvement
- No file I/O during gameplay

**Impact:** Medium effort, high benefit

---

### 10. **SDL Colors as constexpr**
**Current (SDLGraphic.hpp):**
```cpp
SDL_Color customWhite = { 255, 248, 227, 255};
SDL_Color customBlack = { 23, 23, 23, 255};
```

**Recommendation:**
```cpp
static constexpr SDL_Color customWhite{255, 248, 227, 255};
static constexpr SDL_Color customBlack{23, 23, 23, 255};
```

**Benefits:**
- Compile-time constants (no runtime initialization)
- Clear that these never change
- Potential compiler optimizations

**Impact:** Trivial change, free optimization

---

## 📊 Modernization Priority

| Priority | Item | Risk | Benefit | Effort |
|----------|------|------|---------|--------|
| ✅ DONE | Library array → std::array | Low | Medium | Low |
| ✅ DONE | GameState pointers → references | Low | High | Medium |
| 🔥 HIGH | Random number generation | Low | High | Low |
| 🔥 HIGH | NCurses file caching | Medium | Very High | Medium |
| 🟡 MEDIUM | Snake array → std::array | Low | Medium | Low |
| 🟡 MEDIUM | SDL colors constexpr | Low | Low | Trivial |
| 🟡 MEDIUM | getFoodChar() → string_view | Low | Low | Low |
| 🟢 LOW | enum direction → enum class | Medium | Medium | High |
| 🟢 LOW | Particle vector reserve() | Low | Low | Trivial |

---

## 🎯 Recommended Next Steps

1. **Fix random number generation** (Utils.cpp) - Easy win, big perf boost
2. **Cache ASCII art files** (NCursesGraphic.cpp) - Already on your TODO list
3. **Make SDL colors constexpr** - 5-minute change
4. **Convert Snake array to std::array** - Low risk modernization
5. **Consider enum class Direction** - Save for major refactor

---

## ✨ What's Already Modern

- ✅ **C++20 standard** - You're using modern language features
- ✅ **RAII everywhere** - No manual new/delete
- ✅ **Smart constructors** - Deleted defaults where appropriate
- ✅ **Rule of zero/five** - Proper copy/move semantics
- ✅ **const correctness** - Good use of const throughout
- ✅ **Range-based for loops** - Modern iteration patterns
- ✅ **auto where appropriate** - Type inference in chrono code
- ✅ **nullptr instead of NULL** - Modern null pointer constant
- ✅ **Virtual destructors** - Proper polymorphic cleanup

---

## 📝 Notes

The codebase is already quite modern! The changes made today (array + references) were the most impactful. The remaining items are either:
- **Performance optimizations** (random gen, file caching)
- **Minor style improvements** (constexpr, enum class)
- **Future-proofing** (std::array for algorithms)

Your architecture with dynamic library loading and state machines is solid. The next biggest wins would be the performance items (random gen + file caching).
