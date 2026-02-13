# Rosario - Devlog - 1

## Table of Contents
1. [It Was Just a Phase](#11---it-was-just-a-phase)
2. [Back to the Cocoon (Which Would Make More Sense if the Main Entity of the Game Was a Worm Instead of a Snake, But You Get the Idea)](#12-back-to-the-cocoon-which-would-make-more-sense-if-the-main-entity-of-the-game-was-a-worm-instead-of-a-snake-but-you-get-the-idea)
3. [The Snake and the Mouse](#13-the-snake-and-the-mouse)
4. [It's Not a Sword, It's a Key!](#14-its-not-a-sword-its-a-key)
5. [Tearing My Screen Appart](#145-tearing-my-screen-appart)



<br>
<br>

# 1.1 - It Was Just a Phase
Here we are, on the other side of what has been tagged as `V1`. Today the two week sprint towards a more mechanically complex, raylib-unified, *snake* based game start, which means that we're having **R E F A C T O R I N G**  for breakfast, lunch and dinner. I've been in this position before, having to plan and layout carefully thought steps to scale *down* a build so that a new development process can start anew from a compact, controlled state of things. Personally, I still find this situations a bit overwhelming, mostly because after spending so much time writing a game that builds and launches and works and doesn't break and explode into pieces, taking it appart feels like playing a risky game of *Jenga*. Or maybe the analogy is not quite precise, because once you take out one piece the whole thing collapses and the next hours are comprised of a mixture of "might as well keep getting all the stuff I think should be out of the new starting point" and "how can I make this compile again for the love of everything sacred". I've being a little overdramatic, I know (must say that if you've arrived to this log after going through all the development journaling done for `V1`, you already know the extent of my dramatism), but it can get tyring. Theres a counterpoint, of course, the same one that's always looming in any programming journey: nothing works until it works, and when it works life becomes wonderful. So, yeah:
- today's first task is **scaling down and stripping the project of what has become dead weight under the banner of `V2`**
- then, a new build pipeline and some refactoring in the surviving code needs to be done so that a raylib-based game persists
- after which, some porting needs to be done, specially regarding `SDL` stuff
    - for now, I want what was the `SDL` menu in `V1` to be the main menu in `V2`, which entrails adding a `2D` rendering mode/pipeline
    - this means that the particle system, which was tied exclusively to `SDL`, needs to be ported to `Raylib`
- we'll end with a general check, a test suite reconfigration to adapt it in the new `Makefile` and a new, fresh starting point.

If everything goes right, at the end of today there will be a **raylib-exclusive *nibbler* build with the SDL starting menu and gameover and a recovered gtest suite**. If not... doesn't matter, because we will **S U C C E E D**.

> *The good news is that scaling down means also simplifying, which brings solace to the soul. Soulace, if you will.*

<br>
<br>

# 1.2. Back to the Cocoon (Which Would Make More Sense if the Main Entity of the Game Was a Worm Instead of a Snake, But You Get the Idea)
On second thought, it wouldn't even make that much sense if it was a worm. If it was a butterfly, going *back* to the cocoon would totally make sense, but worm would go *foward* to a cocoon. Uhm, anyway, here's a list of things that we're going to lose along the way:
- Anything `SDL` and `NCurses` related
- The graphic interfacing (no longer needed in the new monolibrary approach)
- Any audio related stuff (will rebuild audio inside `Raylib`)

All of this with the consequential changes each removal has in the code. Once done, a new `Makefile` will be written, and we *should* be good... And we are!! If you go through the codebase, you'll see that we're now `Raylib` exclusive and that the game builds and runs smoothly. Yay!

The more tricky part is what comes next, the porting of some `SDL` stuff into Raylib. We'll need a couple of things:
- A 2D-camera based rendering pipeline
- A rewriting of the `ParticleSystem` and the `menu`/`gameover` renderings into `Raylib`
- A general reorganization of the directory structure, some renaming, that kind of managing work...

After some work, I've arrived to a new base build for `V2`, a combination of what was already in place for the `Raylib` version in `V1` with the menuing of `SDL`. Making this work was a hustle, but the byproduct is that I now have implemented a `2D` rendering pipeline alongisde the existing `3D` one, which is nice progress. I've also divided the `srcs` directory intro `AI`, `core` and `graphics`, and will move on from here with a system-based approach in mind. This might mean some more refactorization, as the current division between `Renderer`, `TextRenderer` and `TitleHandler` feels a little bit wonky, but we'll see. 

Be what it may, what I'd like to do with the rest of the day, now that a lot of porting and refactoring is behind me, is work in some new implementations (the **I R K***). Some things in my immediate list:
- A clickable button system for menuing
- A general menu system that handles start, pause and gameover screens
- A systematized pipeline to write stuff in the likes of the current game logo

BUT as I lay out plans, I realize something: I need a deep re-structuring process. The current build is quite messy: there are nested systems that are general-purpose, subsystems that need to be decoupled, uncertainty regardin ownership... So the priority shifts that way. Before we move on, there needs to be a refactoring so that **`Main` owns all systems and calls their update methods in the game loop depending on the state of the game**. And the initial system configuration is comprised of:

- [x] Renderer
- [x] Game controller
- [x] Particle system
- [x] Text system
- [x] Animation System
- [x] Menu system
- [x] InputManager
- [ ] Custom text system/pipeline/subsystem- [ ] Custom text system/pipeline/subsystem

Inter-system communication will be laid out by passing references. If things get too complicated down the line, which I doubt but who am I to say, I'll transition into an event system with connecting lambda functions. I'll get into system rebuild mode. Wish me luck.

## 1.3 The Snake and the Mouse
Let's implement buttons. First, I will disable the keyboard based inputs, as those should be rewritten later to follow the selection of the mouse, i.e., the mouse will leave a selected button once a hovering event is detected, and from there the keyboard will be enabled to move along the menu. Everything regarding a menu will be reworked into a new `MenuSystem`, which will home a new `Button` class and will track it's own state to know if it should render the start menu, the pause options, the gameover screen, etc. This means both creating a new systen and decoupling a substantial amount of managing code that is spread in other parts of the game, like the input handling in the main game loop (although, when keyboard input is recovered the keyhooks might return there; either that or maybe decouple all input into an `InputManager`), and the menu rendering calls in the general `Renderer` system. And after some work, this is the result:
```cpp
struct Button {
    Rectangle bounds;
    std::string Text;

    Color outlineColor;
    Color backgroundColor;
    Color textColor;
    Color hoverColor;
    Color textHoverColor;
    Color outlineHoverColor;

    float outlineWidth = 5.0f;

    std::function<void()> onClick;

    bool isHovered(Vector2 mousePos) const {
        return CheckCollisionPointRec(mousePos, bounds);
    }

    void render(bool hovered) const {
        Color currentColor = hovered ? hoverColor : backgroundColor;
        Color currentTextColor = hovered ? textHoverColor : textColor;
        Color currentOutlineColor = hovered ? outlineHoverColor : outlineColor;
        DrawRectangleRec(bounds, currentColor);
        DrawRectangleLinesEx(bounds, outlineWidth, currentOutlineColor);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), Text.c_str(), 20, 1.0f);
        Vector2 textPos = {
            bounds.x + (bounds.width - textSize.x) / 2,
            bounds.y + (bounds.height - textSize.y) / 2
        };
        DrawTextEx(GetFontDefault(), Text.c_str(), textPos, 20, 1.0f, currentTextColor);
    }
};
```

```cpp
enum class MenuState {
	Start,
	Paused,
	GameOver,
	Options
};

class MenuSystem {
	private:
		GameController &gameController;
		MenuState currentState;
		std::vector<Button> buttons;

		// menu specific particle states
		float particleSpawnTimer;
		const float particleSpawnInterval = 0.15f;
		int logoSnakeTrailCounter;

		// cached screen dimensions
		int screenWidth;
		int screenHeight;

		//helpers
		void spawnMenuParticles(float deltatime, ParticleSystem& particles);
		void initializeButtons();
		void clearButtons();

	
	public:
		MenuSystem(GameController &gameController);;
		~MenuSystem() = default;

		void init(int width, int height);
		void setState(MenuState newStat);
		MenuState getstate() const { return currentState; }

		// update and render for each menu state
		void update(float deltaTime, ParticleSystem& particles, AnimationSystem& animations);
		void render(Renderer &renderer, TextSystem& textSystem,
					ParticleSystem& particles, AnimationSystem& animations,
					const GameState& state);
		void renderGameOver(Renderer &renderer, TextSystem& textSystem,
							ParticleSystem& particles, AnimationSystem& animations,
							const GameState& state);
		void handleInput(Vector2 mousePos, bool mouseClicked);
		Button* getHoveredButton(Vector2 mousePos) const;

		// button functions
		void startGame();
		void switchConfigMode();
		void restartGame();
		void quitGame();
};
```

> For definition of the menu system, go to its cpp file. Also, some stuff is missing at this time, like the pause menu rendering, as that screen/menu is not yet worked.

The general work of the new mouse navigation its not complex. Buttons are rendered via `Raylib` drawing functions to make different kinds of rectangles, and their display changes based on a simple hovering detection to add some basic flair. The handling of the *click* itself is still in `Main`, inside the loop, as the possibility of a `InputManager` is still that, a possibility, but the rest of the necessary managment is on the side of the `MenuSystem` + `Button` combination. `Raylib` gives quite an easy pipeline to manage this:
- `CheckCollisionPointRec(Vector2 point, Rectangle rec)` compares the coordinates of the mouse pointer against a premade rectangle object
- `IsMouseButtonPressed(int button);` is the basic hook for mouse buttons.

Combine these and **a hovering based mouse input management** can easily be coded. The buttons just need an attacked `onClick()` function that is tied to an `std::function`, which is set up during button initialization via lamba-based calls to premade input handling methods and the transition to mouse navigation is done. Everything works, but (BUT) the input handling feels wrong. So, you guessed it right, it's decision making time: let's add another system for input management!

<br>
<br>

## 1.4. It's Not a Sword, It's a Key!
In my mind, this is a quote from *Kingdom Hearts*. If it's not, it surely could be, so I guess that's the point. *I's me, he says!*. Whatever, it has *key* in it and we're going to do keyboard related stuff so etc., let's design an all input encompasing manager. To build this new system, we'll need:
- Track the input context, so that different contexts can have different input outcomes
- Split the input processing in two ways: **navigation** and **gameplay**
- Track the mouse state (for convenience, mainly)

The main structural decision in the manager will be to build the `update()` function around a context check. Menu states will be handled through `NavigationAction`, Gameplay states through a specific polling function and Pause, which is not yet a menu, will be placeholded to work as it has been doing until now, as a set and unset integration. The key aspect here is that **buttons will be initialized with `onClick()` functions** and **keys will be tied to context-depending behaviours**. So, for example, the `W` key will work as an upwards navigation in a menu, cycling through buttons, but will move the secondary snake in the upwards direction during gameplay. Gameplay wise, the only thing that's changed is the place in which the polling is managed, but it is still called in main, it's just that it will be called now via the `InputManager`, then buffered in the `GameController`. Mouse related stuff will be handled via `updateMouseState()`, which refreshes the cursor's position and the state of the left and right mouse buttons, and stores the delta value since last update (not really used atm, but could be put to interesting interaction later, we'll see).

The trickiest part is how to connect what are now button tied, menu system contained functions to key callbacks. That's where `registerNavigationCallback()` and `registerMouseCallback()` come into play. This needs some lambda magic, as well as some changes in the menu system itself, which has gone from a single `handleInput()` function to a two pipeline handling, made up of `handleNavigation()` and `handleMouseInput()`:
```cpp
InputManager inputManager;
	inputManager.registerNavigationCallback([&menu](NavigationAction action) {
    	menu.handleNavigation(action);
	});
	inputManager.registerMouseCallback([&menu](Vector2 pos, bool clicked) {
		menu.handleMouseInput(pos, clicked);
	});
```
Now, how this works needs its own subsection. Or, really, I am the one who needs it >_>

### 1.4.1. Lambda Juggling: A Breakdown of the Callback System (Because I Spent Too Long Understanding This)

Okay, so here's the thing: I spent a good chunk of time trying to write a lambda/callback setup and wrapping my head around how the hell could I make `InputManager` talk to `MenuSystem` without directly knowing about it. I've already written code like this, but it's still something that I have't mastered, one of those things that makes perfect sense once you get it, but until then it feels like arcane powers. So let me break it down for future me (and anyone else who needs it):

#### **The Problem**

The `InputManager` needs to tell the `MenuSystem` about input events (like "user pressed Up" or "user clicked the mouse"), but we DON'T want `InputManager` to directly depend on `MenuSystem`. Why? Because then `InputManager` becomes a mess if we ever want to add a `PauseMenu` or `OptionsMenu` or whatever. We want `InputManager` to be independent, it just needs to know that "input happened" and to tell it to *someone*, *somewhere*.

#### **The Lambda Solution**

Let's dissect this monstrosity:

```cpp
inputManager.registerNavigationCallback([&menu](NavigationAction action) {
    menu.handleNavigation(action);
});
```

**What's happening here?**

1. **`registerNavigationCallback(...)`** is a function in `InputManager` that takes a `std::function<void(NavigationAction)>` as a parameter. In plain English: "Give me a function that takes a `NavigationAction` and returns nothing."

2. **`[&menu]`** is the **capture clause** of the lambda. This is where the magic happens. It's saying: "Hey lambda, you need to *remember* the `menu` object from the surrounding scope, and keep a reference to it (`&`)." Without this, the lambda wouldn't know what `menu` is when it gets called later.

3. **`(NavigationAction action)`** is the lambda's parameter list. When this lambda eventually gets called (by `InputManager`), it will receive a `NavigationAction` value (like `NavigationAction::Up` or `NavigationAction::Confirm`).

4. **`menu.handleNavigation(action);`** is the lambda's body. This is what actually happens when the lambda executes: it takes the `action` it received and forwards it to the `menu` object's `handleNavigation()` method.

#### **The Flow: How It All Connects**

Here's the journey of a keypress from hardware to menu action:

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. USER PRESSES "UP" KEY                                        │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 2. InputManager::update() detects the keypress                  │
│    - Checks keyboardMappings[NavigationAction::Up] == KEY_UP    │
│    - Sees that IsKeyPressed(KEY_UP) is true                     │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 3. InputManager calls: onNavigation(NavigationAction::Up)       │
│    - onNavigation is a std::function member variable            │
│    - It was SET during registration in main.cpp                 │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 4. The LAMBDA executes (the one we registered in main.cpp)      │
│    - Lambda receives: action = NavigationAction::Up             │
│    - Lambda has captured reference to 'menu' object             │
│    - Lambda executes its body: menu.handleNavigation(action)    │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│ 5. MenuSystem::handleNavigation(NavigationAction::Up) runs      │
│    - Switches on the action                                     │
│    - Calls selectPreviousButton()                               │
│    - Button selection changes!                                  │
└─────────────────────────────────────────────────────────────────┘
```

#### **The Storage: How the Lambda Lives**

Inside `InputManager`, we have:

```cpp
std::function<void(NavigationAction)> onNavigation;
std::function<void(Vector2, bool)> onMouseInput;
```

These are **member variables** that store function objects. When we call `registerNavigationCallback()`, we're doing this:

```cpp
void InputManager::registerNavigationCallback(std::function<void(NavigationAction)> callback) {
    onNavigation = callback;  // Store the lambda for later!
}
```

So the lambda we created in `main.cpp` gets **copied into** the `onNavigation` member variable. It lives there, with its captured `&menu` reference intact, waiting to be called.

Later, in `InputManager::update()`, when we detect input:

```cpp
if (IsKeyPressed(key)) {
    if (onNavigation) {  // Check if a callback was registered
        onNavigation(action);  // CALL THE STORED LAMBDA!
    }
}
```

#### **The Mouse Case**

```cpp
inputManager.registerMouseCallback([&menu](Vector2 pos, bool clicked) {
    menu.handleMouseInput(pos, clicked);
});
```

Same pattern:
- Lambda captures `&menu` reference
- Takes two parameters: `Vector2 pos` and `bool clicked`
- Forwards them to `menu.handleMouseInput(pos, clicked)`
- Gets stored in `onMouseInput` member variable
- Called in `InputManager::update()` when mouse state changes

#### **The Capture Clause: Why `&menu` and Not `menu`?**

This tripped me up for a bit. The difference:

- **`[menu]`** (capture by value): Makes a COPY of the menu object. Bad idea — menu is huge, and we'd be copying it every time.
- **`[&menu]`** (capture by reference): Stores a REFERENCE to the original menu object. Lightweight and correct.

The only danger with `[&menu]` is if the `menu` object gets destroyed before the lambda is called. But in our case, `menu` lives for the entire program lifetime in `main()`, so we're safe.

#### **A Simplified Explanation If (IF!!! NEVER WHEN!!!) I Forget This**

1. We create a lambda in `main.cpp` that captures a reference to `menu`
2. We pass that lambda to `InputManager` via `register*Callback()`
3. `InputManager` stores it in a `std::function` member variable
4. When input happens, `InputManager` calls the stored lambda
5. The lambda forwards the input to `menu.handleNavigation()` or `menu.handleMouseInput()`
6. Menu does its thing

**IN EVEN SIMPLER, EVEN MORE HUMAN, PERHAPS EVEN MORE UNDERSTANDABLE TERMS:**
- InputManager has a couple of function attributes that at some point need to be defined. These are onNavigation and onMouseInput
- The moment in which they are defined is right after instantiating the InputSystem in main, and they're astored via the two callback registration functions that the InputManager also has. These registration functions take functions as arguments and store them in the attributes stated in the last point.
- The definition itself is done via lambda functions, right in place, and these lambda functions in main take a reference to menu and basically take some arguments and send them to the respective handling functions in menu.
- What this means is that after doing all of this, InputManager has a couple of function attributes that have the menu scope captured as reference, so when the InputManager update functon, context dependent, calls one of those functions and sends the captured input action (which is a power that only the InputManager has), everything is hoked up to communicate back to menu that this or that action happened.
- The rest is just the menu knowing what to do with the input sent back by the InputManager.

It's basically a **hand-wired event system** without needing a full event bus. The lambda acts as a **bridge** between the generic `InputManager` and the specific `MenuSystem`, letting them communicate without coupling them together.

> Maybe I should build an event bus/system? You tell me. Last time I built a game engine from scratch I did with an event pipeline, but it felt a little bit overdone, so I wanted to try a different approach here. We'll see if this decision bites my ass.

> The important point of all of this: INPUT MANAGER DONE!!
<br>
<br>

## 14.5 Tearing My Screen Appart
I've noticed something terrible while testing the new `V2` starting build: even though I launch the `Raylib` window with `Vsync` on, I'm having a horrible screen tearing effect. My best guess as to why is that there might be too many draw calls, so let's try to attack it from that front. The most likely culprit: too many calls to `BeginDrawing()-EndDrawing()`. Fixing this means rewriting the main game loop so that the `update` and the `rendering` phases are separated. 

### First Attempt: Unified Render Phase

The new loop separates update and render clearly:
```cpp
while (state.isRunning) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameTime = currentTime - lastTime;
		float deltaTime = frameTime.count();
		lastTime = currentTime;
		
		// update phase
		inputManager.update();
		
		switch (state.currentState) {
			case GameStateType::Menu: {
				gameOverStateInitialized = false;
				inputManager.setContext(InputContext::Menu);
				menu.update(deltaTime, particles, animations);
				break;
			}

			case GameStateType::Playing: {
				inputManager.setContext(InputContext::Gameplay);
            	Input input = inputManager.pollGameplayInput();
				
				if (input == Input::Pause) {
					state.isPaused = !state.isPaused;
					state.currentState = state.isPaused ? 
						GameStateType::Paused : GameStateType::Playing;
				}
			
				gameController.bufferInput(input);

				state.timing.accumulator += deltaTime;
				
				while (state.timing.accumulator >= FRAME_TIME) {
					gameController.update();
					state.timing.accumulator -= FRAME_TIME;
					
					if (!state.isRunning) {
						state.currentState = GameStateType::GameOver;
						state.isRunning = true;
						break;
					}
				}
				break;
			}
				
			case GameStateType::Paused:
				inputManager.setContext(InputContext::Paused);
				// No update needed while paused
				break;
				
			case GameStateType::GameOver: {
				if (!gameOverStateInitialized) {
					menu.setState(MenuState::GameOver);
					gameOverStateInitialized = true;
				}
				
				inputManager.setContext(InputContext::GameOver);
				particles.update(deltaTime);
				animations.updateTunnelEffect(deltaTime);
				break;
			}
		}
		
		//rendering phase
		BeginDrawing();
		ClearBackground(Color{23, 23, 23, 255});
		
		switch (state.currentState) {
			case GameStateType::Menu: {
				BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
				menu.render(renderer, textSystem, particles, animations, state);
				EndMode2D();
				break;
			}

			case GameStateType::Playing:
			case GameStateType::Paused: {
				// Update renderer state
				renderer.render(state, state.isPaused ? 0.0f : deltaTime);
				
				// 3D gameplay rendering (Paused uses same render, just frozen)
				BeginMode3D(renderer.getCamera());
				renderer.drawGroundPlane();
				renderer.drawSnake(state.snake_A, snakeAHidden, 
					snakeALightFront, snakeALightTop, snakeALightSide,
					snakeADarkFront, snakeADarkTop, snakeADarkSide);
				
				if (state.config.mode == GameMode::MULTI) {
					renderer.drawSnake(state.snake_B, snakeBHidden,
						snakeBLightFront, snakeBLightTop, snakeBLightSide,
						snakeBDarkFront, snakeBDarkTop, snakeBDarkSide);
				} else if (state.config.mode == GameMode::AI) {
					renderer.drawSnake(state.snake_B, snakeAIHidden,
						snakeAILightFront, snakeAILightTop, snakeAILightSide,
						snakeAIDarkFront, snakeAIDarkTop, snakeAIDarkSide);
				}
				
				renderer.drawFood(state.food);
				EndMode3D();
				
				// UI overlay
				DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, customWhite);
				DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, customWhite);
				DrawFPS(screenWidth - 95, 10);
				
				if (state.isPaused) {
					DrawText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customBlack);
				}
				
				// Post-processing
				renderer.drawNoiseGrain();
				break;
			}
				
			case GameStateType::GameOver: {
				BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
				menu.renderGameOver(renderer, textSystem, particles, animations, state);
				EndMode2D();
				// renderer.drawNoiseGrain(); // keeping noise for contrast in game over
				break;
			}
		}
		
		EndDrawing();
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
```

Aaaaand it didn't fix the tearing :D

Following the cliché, the first suspect is never the real culprit. But we have a second one: I've been dragging along a sleep call at the end of the main loop. It's been there since the multilibrary `V1` project because at some point during development I needed for... I don't even remember. Thing is it might have been messing up the refresh rate controlled by `Raylib`, so after getting it out from the loop... No more apparent tearing. C A S E  S O L V E D. I guess that millisecond was making the render thread hang for, well, a millisecond, causing a visual tear. Amazing stuff, really.

<br>
<br>