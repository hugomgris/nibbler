# Nibbler - Devlog - 1

## Table of Contents
1. [Day Two Plan](#21---day-two-plan)
2. [Runtime Switching](#22---runtime-switching)
    1. [Splitting the `testlib`](#221---splitting-the-testlib)
    2. [Expanding `Main`](#222---expanding-main)

<br>
<br>
<br>

# 2.1 - Day Two Plan
With the basic architecture in place, thanks to which the nibbler executable can load and manage `.so` files at runtime, the main objectives for this second development day are:
- Implement and test **runtime library switching**
- Set up the build pipelines (through Makefile) to fetch the chosen graphic libraries and clone the necessary files for a fresh compilation
- Move from dummy testlibs to basic implementations of the three chosen libraries

**Runtime Switching** will be the toughest step, I feel, as it will definately require a jump in complexity regarding the current basic build. The reward will be, though, equally substantial, as after achieving this step there will trully be a fairly robust build for what nibbler asks, and from that point on the main focus will be able to turn to game design and graphics development. **Makefile stuff **will be quite straightforward: check if the required files are in the repository (won't in a fresh compilation case), fetch and clone them, make the `.so` file build go through them and link the project's dynamic libraries against the graphic libraries, compile the binary. The last step on this Day Two,** moving to basic implementations of the chosen libraries** should also be simple, although I guess it will depend on each library; we'll see down the line (down the documet).

<br>
<br>
<br>

# 2.2 - Runtime Switching
To achieve testing capabilities for runtime switching, the following steps must be taken:
- Split the `testlib` into three different dummies
- Slightly tweak each testlib so the log-based testing is can be easily read and checked.
- Rewrite `Main` so that it both has the necessary tools for Runtime Switching and a basic, finite loop for the testing


## 2.2.1 - Splitting the `testlib`
Moving from a single testlib into three replicas is just a matter of doubling the code, numbering the class names and logs and color coding their respective outputs to make the console checks easier. The following code will be replicated with suffixes `2` and `3`, and color output will go through the newly added header `colors.h`:
```cpp
#include "../../incs/IGraphic.hpp"
#include "../../incs/colors.h"
#include <iostream>

class TestLib1 : public IGraphic {
	public:
		void init(int width, int height) override {
			std::cout << BRED << "[TestLib-1]" << RESET <<  " init(" << width << ", " << height << ")" << std::endl;
		}

		void render(const GameState &state) override {
			std::cout << BRED << "[TestLib-1]" << RESET << " render: snake length - "
				<< state.snake.length << std::endl;
			
				if (state.gameOver)
					std::cout  << BRED << "[TestLib-1]" << RESET << " GAME OVER" << std::endl;
		}

		Input pollInput()  override{
			// This should be nonblocking, but for testing purposes it's ok like this
			return Input::None;
		}
};

extern "C" IGraphic *createGraphic() {
	return new TestLib1();
}

extern "C" void destroyGraphic(IGraphic *g) {
	delete g;
}
```
## 2.2.2 - Expanding `Main`
In order to achieve the objective as quickly as possible, let's just rebuild `Main` so that *it works* regarding Runtime Switching, and we'll refactor later down the line. The new `Main` is larger, but not that complicated. It just needs:
- Input handling (via `termios`)
- A `GraphicLibrary` class wrapper around the previous `Main` code that implements methods to **load and unload libraries**.
- A finite game loop that runs for a set number of "frames", catches user input, simulates some logic and repeats itself after a fixed amount of time.

```cpp
#include "../incs/IGraphic.hpp"
#include "../incs/GameState.hpp"
#include "../incs/colors.h"
#include <dlfcn.h> // for dynamic linking
#include <unistd.h> // for usleep - TESTING
#include <termios.h> // for non-blocking input
#include <fcntl.h>
#include <iostream>

char	getNonBlockingChar() {
	struct termios oldt, newt;
	char ch = 0;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	return ch;
}

// TODO: if this class survives beyond the initial testing phase, it should be in CANONICAL FORM
class GraphicLibrary {
	private:
		void *handle;
		IGraphic *graphic;
		using CreateFn = IGraphic *(*)();
		using DestroyFn = void (*)(IGraphic*);
	
	public:
		GraphicLibrary() : handle(nullptr), graphic(nullptr) {}

		bool load(const char* libPath) {
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

		void unload() {
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

		IGraphic *get() { return graphic; }
		~GraphicLibrary () { unload(); }
};

int main() {
	const char *libs[] = {
		"./testlib1.so",
		"./testlib2.so",
		"./testlib3.so"
	};
	int currentLib = 0;

	GraphicLibrary gfxLib;
	if (!gfxLib.load(libs[currentLib]))
		return 1;

	gfxLib.get()->init(20, 20);

	Vec2 segments[4] = {{10,10}, {9,10}, {8,10}, {7,10}};
    SnakeView snake { segments, 4 };
    FoodView food{ {5,5} };
    GameState state { 20, 20, snake, food, false };

	std::cout << BCYN << "\nPress 1/2/3 to switch libraries, 'q' to quit\n" << RESET << std::endl;

	// TESTING GAME LOOP
	for (int frame = 0; frame < 1000; ++frame) {
		char key = getNonBlockingChar();

		if (key == 'q') {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			break;
		}

		if (key >= '1' && key <= '3') {
			int newLib = key - '1';
			if (newLib != currentLib) {
				std::cout << BMAG << "\nSwitching from lib " << (currentLib + 1) << " to lib " << (newLib + 1) << std::endl << std::endl;

				// 1- Destroy the current graphic
				gfxLib.unload();

				// 2- Make the actual switch
				if (!gfxLib.load(libs[newLib])) {
					std::cerr << BRED << "Failed to load new library!" << RESET << std::endl;
					return 1;
				}

				gfxLib.get()->init(20, 20);
				currentLib = newLib;
			}
		}

		// Re-render
		gfxLib.get()->render(state);

		// Game logic simulation
		if (frame % 10 == 0 && snake.length < 10) {
			snake.length++;
			state.snake = snake;
		}
		
		// 0.5 s delay
		usleep(500000);
	}

	return 0;
}
```

This new main achieves exactly what it aimed to: every half a second, a `testlib` log is printed to the console, tagged with the number and color respective to the current library. Pressing `1/2/3` keys switches the library (with switch and load logs), and every 10 frames the snake grows one unit. Pressing `q` key exits the execution with a farewell message. It looks like this:

<img src="./RuntimeSwitchingTestLogs.png" alt="Runtime Switching Test Logs" width="300"/>

Now, let's break down some of the new stuff added to `Main`:
- The `input polling` in this test is done via `termios.h`, but the polling itself will be delegated to each implemented library's own event polling. Said polling will undergo a translation from library-specific inputs to the `Input` enum, returning special values for library swithching (e.x., `Input::SwitchLib1`). Nevertheless, as I'm not that familiar with `termios.h`, let's examine how to use it for simple non-blocking input polling:
    - `termios` is the Linux/Unix **terminal I/O** control system. In other words, is the system's tool to process keyboard input while in terminal. I guess the name is quite self-explanatory: TERMinal InputOutput Stream.
    - Normally / By default, terminal input is:
        - **Canonical Mode**: Waits for Enter key, bufers the whole line
        - **Echoing**: Shows what you type on screen
        - **Blocking**: `getchar()` pauses program until you type something
    - But the testing needs:
        - **Non-canonical mode**: Read each keypress immediately
        - **No echo**: Don't show what's typed
        - **Non-blocking**: Don't pause execution if no key is pressed
    - To achieve this, the following bits are put in place:
        -`struct termios odlt, newt` → Terminal setting structures
        -`tcgetattr` → Stores file descriptor for standard input and original terminal settings
        - `newt.c_lflag &- ~(ICANNON | ECHO)` → Turn OFF canoncail mode and echo:
            - `c_lflag` = "local flags" (terminal behavior flags)
            - `ICANNON` = cannonical mode (line buffering)
            - `ECHO` = echo typed characters
            - `~(...)` = bitwise NOT (turns flags OFF)
            - `&=` = bitwise AND assignment
        - `tcsetattr(STDIN_FILENO, TCSANOW, &new)` → Apply new settings immediately (TCSANOW means, well, **NOW**).
        - `int oldf = fcntl(STDIN_FILENO, F_GETFL, 0)` → Get current file status flags
        - `fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK)` → Add `O_NONBLOCK` flag (don't wait for input, return immediately)

<br>

- Library switching goes through this detailed flow:
    - Unload the current library
        - Destroy the current displayed graphic (delete windows, free SDL resources, etc.)
        - Unload the `.so` from memory
    - Load new library
        - Load new `.so`
        - Create new `IGraphic` object (new SDL window, SFML window, ncruses screen, etc.)
    - Reinitialize
        - Call `init()` on the new library with initially set up dimensions (program run arguments)
        - Draw next frame with the new library while preserving Game State.

> In the future, when real libraries are implemented, **a pause during switch might be needed** to prevent the snake from moving while the screen is rebuilding.

> Question to self: **Will the switching be visually translated in some kind of screen flashing or hiccup?** If so, I'll need to research how to avoid it, if possible at all.

    
