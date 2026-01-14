# Nibbler - Devlog - 3

## Table of Contents
1. [Day Three Plan](#21---day-three-plan)
2. [Library-Side Input Polling](#32---library-side-input-polling)
3. [A Game Is Born](#33---a-game-is-born)
4. [A Main Is Refactored](#34---a-main-is-refactored)

<br>
<br>
<br>

# 3.1 - Day Three Plan
Two main things in mind for today's work session:
- Transitioning to library-side input polling
- Coding a micro interaction with the game (a moving, controllable snake for starters)

Now that the project's skelleton is almost done, I want to reach a point in which the library switching can happen without the need to be focusing on the terminal, which is what happens now due to the game-side, termios-based input polling, and then start having fun with a green, moving squiggle. Should be simple enough, and I'm eager to start designing some stuff, so let's just get into work mode.

<br>
<br>
<br>

# 3.2 - Library-Side Input Polling
Well, this end up being really *trivial*. I write this with a *"wow, that was unexpected"* undertone because I spent yesterday's evening mind-munching on how to do this, thinking about symbol handling and what not, and in the end all that was needed was moving from an `int` based input polling (the `termios` temporary one) into a `Input::` based catching by calling the polling function on the `IGraphic` pointer inside `Main`. In code:
```cpp
Input input = gfxLib.get()->pollInput();

		if (input == Input::Quit) {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			running = false;
			break;
		}

		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = (int)input - 1;

			if (newLib != currentLib) {
				std::cout << BMAG << "\nSwitching from lib " << (currentLib + 1) 
						<< " to lib " << (newLib + 1) << std::endl << std::endl;

				// 1- Destroy the current graphic
				gfxLib.unload();

				// 2- Make the actual switch
				if (!gfxLib.load(libs[newLib])) {
					std::cerr << BRED << "Failed to load new library!" << RESET << std::endl;
					return 1;
				}

				gfxLib.get()->init(width, height);
				currentLib = newLib;
			}
		}
```
Wow, what an incredible change, I know. Whatever, now the polling handled where it needs to be handled (so first objective: donezo) and I can switch libraries without the need of terminal focusing. I also added an `Escape` key case to each library so that it returns the same input case as the `q`, and now the game can be shut down with both `ESC` and `Q`. 

Having said that, however, but, nevertheless, the fact that i didn't arrive at this conclusion right on and had to do all that mentioned mind-munching is significant, so I'll take the opportunity this section gives me to take a step back and refresh on some concepts. After the (late) realization, I mind-munched again about why there was a need for symboling in the first place, but this is quite simple:

> Because the tasks requires full separation between logic and graphics

That's it, there's really not much more to it. At some point there's going to be a need to call graphic-library related functions, so an instance of an object capable of doing that will need to be created, and that means ***knowing*** about the graphic libraries themselves. And as it is imperative that `Main` stays independent and *ignorant* of whatever is happening on the graphics side of the world, some sort of **"bridge"** needs to be built. That's where symboling plays its part:
```
Main (nibbler)              .so file (nibbler_ncurses.so)
┌─────────────┐            ┌──────────────────────────┐
│             │            │                          │
│ GraphicLib  │            │  class NCursesGraphic    │
│   ├─handle  │            │        : public          │
│   └─graphic ├────────────│     IGraphic {...}       │
│      │      │  dlsym()   │                          │
│      │      │            │  extern "C" {            │
│      │      │            │    IGraphic*             │
│      │      │            │    createGraphic() {     │
│      └──────┼──dlsym─────│      return new          │
│             │            │      NCursesGraphic();   │
│             │            │    }                     │
└─────────────┘            │  }                       │
                           └──────────────────────────┘
```
The main reason, besides the task's requirements and whatnot, is the **name mangling** done by the C++ compiler. The mangling itself occurs to support overloading (for example, more than one signature for a single function, changing arguments between them), and all that `extern "C"` stuff is put in place to **compile symbols**, i.e. cancel the mangling. It's just a way to **use C linkage to export manually named symbols**. And this, on its side, is needed becase the symbol fetching function provided by the dynamic library managing include, `dlsym()` asks for a string name, and in the world of name mangling that would be impossible (I think, maybe it would not, but it would surely be hell-worthy).

With this clear, a derivated question: **Why don't interface methods need the compiled symbols**? Easy answer, once again: once `Main` has a `IGraphic*` pointer, managed in the current build via the `GraphicLibrary` intermediary class, `Main` can **call virtual functions through the `vtable`**:
```cpp
IGraphic* graphic = create();  // Got the pointer
graphic->render(state);        // Virtual function call (no dlsym needed!)
     ↑
Virtual dispatch through vtable (C++ magic)
```
I use the word ***magic*** because right now I won't go any further down this rabbit hole, I have a pile of other things to do. Let's just note that **the vtable is set up at object cration by the C++ runtime**, and that schematically it looks like:
```
graphic pointer
    ↓
┌─────────────┐
│ NCursesGfx  │
│   ├─vtable ─┼────┌────────────────┐
│   ├─width   │    │ init()         │
│   └─height  │    │ render()       │
└─────────────┘    │ pollInput()    │
                   │ ~NCursesGfx()  │
                   └────────────────┘
```
So, to sum it up: **Due to Separation of Concerns requirements, a Factory Pattern via `extern "C"` is needed when sides need some why of communicating while keeping themselves ignorant (and independent) from each other. 

Let's move on.


<br>
<br>
<br>

# 3.3 - A Game Is Born
First of all, let's clean `main.cpp`. I want this file to just contain the `main()` function (I will later clean the game loop itself, it's getting quite messy), so I'm going to take the graphic library mediating class to it's own `.hpp/.cpp` combo and rename it to **`LibraryManager`**:
```cpp
#pragma once
#include "IGraphic.hpp"
#include <dlfcn.h>
#include <iostream>

class LibraryManager {
	private:
		void *handle;
		IGraphic *graphic;
		using CreateFn = IGraphic *(*)();
		using DestroyFn = void (*)(IGraphic*);
	
	public:
		LibraryManager();

		LibraryManager(const LibraryManager&) = delete;
		LibraryManager& operator=(const LibraryManager&) = delete;

		~LibraryManager();

		bool load(const char *libPath);
		void unload();

		IGraphic *get();
};
```
```cpp
#include "../incs/LibraryManager.hpp"

LibraryManager::LibraryManager() : handle(nullptr), graphic(nullptr) {}

LibraryManager::~LibraryManager() { unload(); }

bool LibraryManager::load(const char * libPath) {
	handle = dlopen(libPath, RTLD_NOW);
			if (!handle) {
				std::cerr << "dlopen error: " << dlerror() << std::endl;
				return false;
			}
			
			CreateFn create = (CreateFn)dlsym(handle, "createGraphic");
			DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");
			
			if (!create || !destroy) {
				std::cerr << "Symbol error: " << dlerror() << std::endl;
				dlclose(handle);
				handle = nullptr;
				return false;
			}
			
			graphic = create();
			std::cout << "Loaded: " << libPath << std::endl;
			return true;
}

void LibraryManager::unload() {
	if (graphic) {
		using DestroyFn = void (*)(IGraphic*);
		DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");
		if (destroy) {
			destroy(graphic);
		}
		graphic = nullptr;
	}

	if (handle) {
		dlclose(handle);
		handle = nullptr;
	}
}

IGraphic *LibraryManager::get() { return graphic; }
```

After this little rearrangement, the first step towards game-life I want it to be having a moving, controllable snake. To achieve this, some refactoring must be done and a couple of new classes must be born. The main idea here is to have a `snake` class/object created in `Main`, to which the `GameState` will be pointing to. I will also create a `GameManager` class to handle the top most layer of game interaction (at this stage, the direction-based moving of the snake), which will also point to the unique `snake` instance (guess we'll go down singleton's lane). Due to some circular dependency hazards, I also took the `Vec2` struct out of the `GameState.hpp` file into a new `DataStructs.hpp` header. Some of the design choices derive from the separation constrains (for example, I'm avoiding STL containers like they were the Plague, that's why `segments` are array-based instead of `std::vector`-based and `Vec2` is a struct instead of an `std::pair`).The most complex (not complex at all, that is) part of this is the `snake` class design, so let's go deeper into it after collecting the most recent states of all the other files/classes/data that will interface with `snake`:
```cpp
struct Vec2 {
	int	x;
	int	y;
};
```
```cpp
struct GameState {
	int			width;
	int			height;
	Snake*		snake;
	FoodView	food;
	bool		gameOver;
};
```
```cpp
class GameManager {
	private:
		Snake *_snake;

	public:
		GameManager(Snake *snake);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default; // TODO: ?

		void moveSnake();
};
```
```cpp
GameManager::GameManager(Snake *snake) : _snake(snake) {}

void GameManager::moveSnake()  {
	_snake->move();
}
```
The tiny inaugural version of the `snake` class just needs to track a couple of things: **it's length (segment count), the coordinates for each of it's segments (array based) and it's direction (UP/DOWN/LEFT/RIGHT)**. Besides that, to be able to (internally) shout the classic *it's alive!!* there only needs to be a `move()` method that takes into consideration the snake's current direction and moves each segment in the classic, snake-like movement.

This last part just requires tracking of segment positioning before moving the head of the snake, which will always advance in it's current set direction. The head moves, and each other segment ocuppies the pre-stored position of the segment before it. In other words: the head moves, the others follow:
```cpp
enum direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Snake {
	private:
		int			_length;
		Vec2		_segments[100];
		direction	_direction;
		int			_hLimit;
		int			_vLimit;

	public:
		Snake() = delete;
		Snake(int width, int height);

		Snake(const Snake &other);
		Snake &operator=(const Snake &other);
		
		~Snake() = default;

		int getLength() const;
		const Vec2 *getSegments() const;

		void move();
		void changeDirection(direction dir);
};
```
```cpp
Snake::Snake(int width, int height): _length(4), _direction(LEFT), _hLimit(width), _vLimit(height) {
	_segments[0] = {10, 10};
	_segments[1] = {9, 10};
	_segments[2] = {8, 10};
	_segments[3] = {7, 10};
}

Snake::Snake(const Snake &other)
{
	*this = other;
}

Snake &Snake::operator=(const Snake &other) {
	if (this != &other)
	{
		this->_length = other._length;
		for (int i = 0; i < this->_length; ++i)
			this->_segments[i] = other._segments[i];
	}
	return *this;
}

int Snake::getLength() const { return _length; }

const Vec2 *Snake::getSegments() const { return _segments; }

void Snake::move(){
	auto head = _segments[0];
	Vec2 previousPositions[_length];
	for (int i = 0; i < _length; i++) {
		previousPositions[i] = _segments[i];
	}
	
	switch (_direction)
	{
		case LEFT:
			if (head.x > 0) {
				head.x--;
			}
			break;

		case RIGHT:
			if (head.x < _hLimit - 1) {
				head.x++;
			}
			break;

		case UP:
			if (head.y > 0) {
				head.y--;
			}
			break;

		case DOWN:
			if (head.y < _vLimit - 1) {
				head.y++;
			}
			break;
	}

	_segments[0] = head;

	for (int i = 1; i < _length; ++i)
	{
		Vec2 newPos;
		newPos.x = previousPositions[i - 1].x;
		newPos.y = previousPositions[i - 1].y;
		_segments[i] = newPos;
	}
}

void Snake::changeDirection(direction dir) { _direction = dir; };
```
Back in `Main`, timing the calls to `move()` and the key polling to a simple `changeDirection()` setter, the time has come: ***it's alive!!***
```cpp
int main(int argc, char **argv) {
	if (argc != 3)
	{
		std::cout << BYEL << "Usage: ./nibbler <width> <height>" << RESET << std::endl;
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);

	std::cout << BCYN << "Press 1/2/3 to switch libraries, 'q' to quit" << RESET << std::endl;

	const char *libs[] = {
		"./nibbler_ncurses.so",
		"./nibbler_sdl.so",
		"./nibbler_raylib.so"
	};
	int currentLib = 0;

	LibraryManager gfxLib;
	if (!gfxLib.load(libs[currentLib]))
		return 1;

	gfxLib.get()->init(width, height);

	/* Vec2 segments[100] = {{10,10}, {9,10}, {8,10}, {7,10}};
	SnakeView snake { segments, 4 }; */
	Snake snake(width, height);
	FoodView food{ {5,5} };
	GameState state { width, height, &snake, food, false };  // ← Pass pointer

	GameManager gameManager(&snake);

	// TIMING SETUP
	const double TARGET_FPS = 10.0;					// Snake moves 10 times per second
	const double FRAME_TIME = 1.0 / TARGET_FPS; 	// 0.1 seconds per update
	
	auto lastTime = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;
	bool running = true;
	int frameCount = 0;

	// MAIN GAME LOOP
	while (running) {
		// Calculate delta time
		auto currentTime = std::chrono::high_resolution_clock::now();
		double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;
		
		accumulator += deltaTime;

		Input input = gfxLib.get()->pollInput();

		if (input == Input::Quit) {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			running = false;
			break;
		}

		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = (int)input - 1;

			if (newLib != currentLib) {
				std::cout << BMAG << "\nSwitching from lib " << (currentLib + 1) 
						<< " to lib " << (newLib + 1) << std::endl << std::endl;

				// 1- Destroy the current graphic
				gfxLib.unload();

				// 2- Make the actual switch
				if (!gfxLib.load(libs[newLib])) {
					std::cerr << BRED << "Failed to load new library!" << RESET << std::endl;
					return 1;
				}

				gfxLib.get()->init(width, height);
				currentLib = newLib;
			}
		}

		if (input == Input::Up) { snake.changeDirection(UP); }
		else if (input == Input::Down) { snake.changeDirection(DOWN); }
		else if (input == Input::Left) { snake.changeDirection(LEFT); }
		else if (input == Input::Right) { snake.changeDirection(RIGHT); }
		

		// Fixed timestep game logic updates
		while (accumulator >= FRAME_TIME) {
			// Game logic simulation (runs at fixed rate)
			/* if (snake.length < 10) {
				snake.length++;
				state.snake = snake;
			} */

			gameManager.moveSnake();
			
			frameCount++;
			accumulator -= FRAME_TIME;
		}

		// Render (can happen more frequently than updates)
		gfxLib.get()->render(state);

		// Small sleep to prevent busy-waiting and reduce CPU usage
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}
```

Awesome, but this sad, messy state of `Main` is making baby Jesus cry, so let's put our refactor gloves on.

<br>
<br>
<br>

# 3.4 - A Main Is Refactored
First of all, let's cl