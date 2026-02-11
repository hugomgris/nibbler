# Nibbler - Devlog - 9

## Table of Contents
1. [Day Nine Plan](#91-day-nine-plan)
2. [Tis All About States](#92-tis-all-about-states)

<br>
<br>
<br>

## 9.1 Day Nine Plan
I've been working on Playdate things, so I arrive at this ninth *snake* day at 16:30 in the evening. This means I have little time and low energy for nibbling stuff, so I'll just focus on implementing the starting menu/waiting state for the game and go home.

## 9.2 Tis All About States
Firstly, there's something to be decided regarding this menu situation. If I keep the game open to be started in any library, I'd have to implement the menu three times, one each. But if I fix the starting point in any of the three libraries, it's going to be `NCurses`, as one of the main aesthetic ideas is for the game to *evolve* visually from ASCII to 3D, and this leaves me with the most annoying, less-option-y option of them all. I'll decide along the way.

Whatever the case, the menu implementation itself follows a transversal pattern, that is: I have to write the necessary flow for the menu to exist in any mode any way, so I'll focus on that first. The approach will be the standard: **State Machine**. So step one: enumerating the states and adding a variable to the `GameState` struct (I'll also add a score variable, *ya que estamos*):
```cpp
enum class GameStateType {
    Menu,
    Playing,
    Paused,
    GameOver
};
```
```cpp
struct GameState {
    int width;
    int height;
    Snake* snake;
    Food* food;
    Input* lastInput;
    bool isPaused;
    bool isRunning;
    bool gameStarted;
    GameStateType currentState;
    int score;
};
```

Then, we need a dedicated function to render the menu, and we ca do so by adding it to the `IGraphic` interface. This also leads me to think that to tie things up and cleanly, I could have the recurring drawing functions across libraries be present in the interface too, like `drawSnake`, `drawFood`, etc. We'll see. For now:
```cpp
class IGraphic {
public:
    virtual ~IGraphic() = default;
    virtual void init(int width, int height) = 0;
    virtual void render(const GameState& state, float deltaTime) = 0;
    virtual void renderMenu(const GameState& state) = 0;  // Add this
    virtual void renderGameOver(const GameState& state) = 0;  // Add this
    virtual Input pollInput() = 0;
};
```
> Added also `renderGameOver` because I'll need it down the line

And we'll focus in the menu and state handling from here on now.

### Input Management for Menus
With the state enum and interface ready, the next step is adding input handling for the **Enter** key, which will be used to navigate through menu states and restart after game over. The `Input` enum gets a new member:

```cpp
enum class Input {
    None,
    SwitchLib1, SwitchLib2, SwitchLib3,
    Up, Down, Left, Right,
    Quit,
    Pause,
    Enter  // New!
};
```

Then each renderer needs to detect Enter key presses. This varies per library due to their different input APIs:

**NCurses** has multiple Enter key codes to handle (because terminals are fun like that):
```cpp
case '\n':      return Input::Enter; // Standard newline
case KEY_ENTER: return Input::Enter; // Keypad enter
```

**SDL2** needs both regular and numpad Enter:
```cpp
case SDLK_RETURN:   return Input::Enter;
case SDLK_KP_ENTER: return Input::Enter;
```

**Raylib** is the most straightforward:
```cpp
if (IsKeyPressed(KEY_ENTER)) return Input::Enter;
if (IsKeyPressed(KEY_KP_ENTER)) return Input::Enter;
```

### The State Machine Implementation
With the foundation laid, it's time to build the actual state machine in `main.cpp`. The flow is:
- **Menu** → Press Enter → **Playing**
- **Playing** → Snake dies → **GameOver**
- **Playing** → Press Space → **Paused**
- **Paused** → Press Space → **Playing**
- **GameOver** → Press Enter → **Menu** (restart)

The main loop gets a big switch statement:

```cpp
switch (state.currentState) {
    case GameStateType::Menu:
        if (input == Input::Enter) {
            state.currentState = GameStateType::Playing;
            accumulator = 0.0;  // Reset time to prevent instant movement
        }
        gfxLib.get()->renderMenu(state);
        break;
        
    case GameStateType::Playing:
        if (input == Input::Pause) {
            state.isPaused = !state.isPaused;
            state.currentState = state.isPaused ? 
                GameStateType::Paused : GameStateType::Playing;
        }
        
        accumulator += deltaTime;
        gameManager.bufferInput(input);
        
        while (accumulator >= FRAME_TIME) {
            gameManager.update();
            accumulator -= FRAME_TIME;
            
            if (!state.isRunning) {
                state.currentState = GameStateType::GameOver;
                state.isRunning = true;
                break;
            }
        }
        
        gfxLib.get()->render(state, deltaTime);
        break;
        
    case GameStateType::Paused:
        if (input == Input::Pause) {
            state.isPaused = false;
            state.currentState = GameStateType::Playing;
        }
        gfxLib.get()->render(state, 0.0f);  // No deltaTime updates when paused
        break;
        
    case GameStateType::GameOver:
        if (input == Input::Enter) {
            // Full restart - create fresh snake and food
            snake = Snake(width, height);
            food = Food(Utils::getRandomVec2(width - 1, height - 1), width, height);
            state.snake = &snake;
            state.food = &food;
            state.score = 0;
            state.gameOver = false;
            state.isPaused = false;
            accumulator = 0.0;
            gameManager.clearInputBuffer();
            state.currentState = GameStateType::Menu;
        }
        gfxLib.get()->renderGameOver(state);
        break;
}
```

The key insight here is **timing management**: resetting the `accumulator` on state transitions prevents the game from executing multiple frames instantly due to built-up deltaTime. This was causing snakes to die immediately on restart.

### Menu Rendering: Three Flavors
With the state machine working, each graphics library needs its own menu implementation. The challenge: **no text rendering libraries loaded**. SDL_ttf would be ideal for SDL, but adding another dependency this late felt like overkill. Instead, I opted for creative solutions per renderer.

### NCurses: The Simple Champion
Terminal rendering is straightforward - just print text! The initial attempt was an elaborate ASCII art title using Unicode box-drawing characters (███╗ ╚═╝ etc.), but terminal compatibility issues made it unreliable. The solution? **Keep it simple**:

```cpp
void NCursesGraphic::renderMenu(const GameState &state) {
    werase(gameWindow);
    int win_height, win_width;
    getmaxyx(gameWindow, win_height, win_width);
    
    drawBorder();
    
    const char* simpleTitle = "N I B B L E R";
    int simpleTitleY = (win_height / 2) - 2;
    int simpleTitleX = (win_width - strlen(simpleTitle)) / 2;
    
    wattron(gameWindow, COLOR_PAIR(4) | A_BOLD);
    mvwaddstr(gameWindow, simpleTitleY, simpleTitleX, simpleTitle);
    wattroff(gameWindow, COLOR_PAIR(4) | A_BOLD);
    
    const char* instructions = "Press ENTER to start";
    mvwaddstr(gameWindow, simpleTitleY + 3, 
        (win_width - strlen(instructions)) / 2, instructions);
    
    const char* controls = "Arrow keys to move | SPACE to pause | Q to quit";
    mvwaddstr(gameWindow, simpleTitleY + 5,
        (win_width - strlen(controls)) / 2, controls);
    
    wnoutrefresh(stdscr);
    wnoutrefresh(gameWindow);
    doupdate();
}
```

Spaced letters, centered text, clear instructions. Sometimes the simplest solution is the best one.

### SDL2: Retro Block Letters
Without SDL_ttf, text rendering becomes an art project. The solution? **Draw text using colored rectangles** to form block letters, giving it a retro arcade feel:

```cpp
// Drawing "NIBBLER" letter by letter with SDL_Rects
// Example: The letter 'N'
SDL_Rect n1 = {centerX - 150, centerY - 80, 15, 60};  // Left vertical
SDL_Rect n2 = {centerX - 150, centerY - 80, 40, 15};  // Top diagonal
SDL_Rect n3 = {centerX - 115, centerY - 65, 15, 60};  // Right vertical

SDL_RenderFillRect(renderer, &n1);
SDL_RenderFillRect(renderer, &n2);
SDL_RenderFillRect(renderer, &n3);
```

Each letter is meticulously crafted from rectangles. Instructions and other text are shown as dotted patterns - less readable but visually consistent. The result looks like something out of a 1980s arcade cabinet.

### Raylib: Text Primitive Paradise
Raylib has built-in text rendering, making it the easiest implementation:

```cpp
void RaylibGraphic::renderMenu(const GameState& state) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    DrawText("NIBBLER", screenWidth/2 - 200, screenHeight/2 - 100, 80, BLUE);
    DrawText("Press ENTER to start", screenWidth/2 - 150, 
        screenHeight/2, 30, WHITE);
    DrawText("Arrow keys to move | SPACE to pause", screenWidth/2 - 200,
        screenHeight/2 + 50, 20, GRAY);
    
    EndDrawing();
}
```

Just works™. The 3D renderer gets the luxury of proper text rendering.

<br>
<br>
<br>

## 9.3 The Phantom Input Bug Saga
With menus working, testing revealed a nasty bug: **snakes would sometimes die instantly on restart** or make sudden turns without player input. The culprit? **Multiple sources of stale state**:

### Issue #1: SDL Food Particle Explosions
SDL tracks the last food position to spawn particle effects when food is eaten. On game restart, the new food position differed from the old one, triggering an explosion. The fix was simple - reset tracking variables in `renderMenu()`:

```cpp
void SDLGraphic::renderMenu(const GameState& state) {
    lastFoodX = -1;
    lastFoodY = -1;
    particles.clear();
    borderLines.clear();
    // ... render menu
}
```

### Issue #2: Accumulated Time
The game accumulates deltaTime to ensure consistent movement speed. But this accumulator kept running during menu state, meaning pressing Enter could trigger multiple immediate game updates. Solution: **reset accumulator on state transitions**:

```cpp
case GameStateType::Menu:
    if (input == Input::Enter) {
        state.currentState = GameStateType::Playing;
        accumulator = 0.0;  // Fresh start!
    }
```

### Issue #3: The Snake Copy Assignment Bug
This was the insidious one. When restarting, `main.cpp` does:
```cpp
snake = Snake(width, height);  // Create new snake with random direction
```

The Snake copy assignment operator looked innocent enough:
```cpp
Snake &Snake::operator=(const Snake &other) {
    if (this != &other) {
        this->_length = other._length;
        for (int i = 0; i < this->_length; ++i)
            this->_segments[i] = other._segments[i];
    }
    return *this;
}
```

**But `_direction` wasn't being copied!** The new snake had fresh body positions but kept the old snake's movement direction. Result: body going one way, moving another way, instant collision with self. The fix:

```cpp
Snake &Snake::operator=(const Snake &other) {
    if (this != &other) {
        this->_length = other._length;
        this->_direction = other._direction;  // The missing line!
        for (int i = 0; i < this->_length; ++i)
            this->_segments[i] = other._segments[i];
    }
    return *this;
}
```

One missing line caused hours of debugging. Classic C++.

<br>
<br>
<br>

## 9.4 Day Nine Retrospective
What started as a simple "let's add a menu" task turned into a comprehensive state machine implementation with cross-renderer menu design and a deep dive into subtle state management bugs. The menu system is now fully functional across all three renderers, each with its own visual flavor:
- **NCurses**: Clean, spaced text with the iconic 4-layer gradient border
- **SDL2**: Retro block letters with particle effects
- **Raylib**: Modern text rendering with 3D aesthetic

The bug fixes were particularly enlightening - they revealed how easy it is for state to leak between game sessions when using object assignment. The Snake direction bug especially demonstrates why **every member variable must be handled in copy/assignment operators**, even ones that seem "obvious."

Next session's focus will be on polish: refining the visual transitions between states, maybe adding some animation to the menus, and ensuring the score tracking is visible and satisfying across all renderers. The core gameplay loop is solid, the rendering is solid, and the state management is finally, *finally* solid.

Time to go home and think about shadow puppets on the Playdate for the whole weekend *huehuehuehue*
