#include "../incs/IGraphic.hpp"
#include "../incs/GameState.hpp"
#include "../incs/colors.h"
#include <dlfcn.h> // for dynamic linking
#include <unistd.h> // for usleep - TESTING
#include <termios.h> // for non-blocking input
#include <chrono>  // for timing/FPS setup
#include <thread>
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

class GraphicLibrary {
	private:
		void *handle;
		IGraphic *graphic;
		using CreateFn = IGraphic *(*)();
		using DestroyFn = void (*)(IGraphic*);
	
	public:
		GraphicLibrary() : handle(nullptr), graphic(nullptr) {}

		GraphicLibrary(const GraphicLibrary&) = delete;
		GraphicLibrary& operator=(const GraphicLibrary&) = delete;

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
		"./nibbler_raylib.so",
		"./nibbler_sdl.so"
	};
	int currentLib = 0;

	GraphicLibrary gfxLib;
	if (!gfxLib.load(libs[currentLib]))
		return 1;

	gfxLib.get()->init(width, height);

	Vec2 segments[4] = {{10,10}, {9,10}, {8,10}, {7,10}};
	SnakeView snake { segments, 4 };
	FoodView food{ {5,5} };
	GameState state { width, height, snake, food, false };

	// TIMING SETUP
	const double TARGET_FPS = 10.0;					// Snake moves 10 times per second
	const double FRAME_TIME = 1.0 / TARGET_FPS; 	// 0.1 seconds per update
	
	auto lastTime = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;
	bool running = true;
	int frameCount = 0;

	// MAIN GAME LOOP
	while (running && frameCount < 1000) {
		// Calculate delta time
		auto currentTime = std::chrono::high_resolution_clock::now();
		double deltaTime = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;
		
		accumulator += deltaTime;

		// Handle input (always responsive, not tied to game updates)
		char key = getNonBlockingChar();

		if (key == 'q') {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			running = false;
			break;
		}

		if (key >= '1' && key <= '3') {
			int newLib = key - '1';
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

		// Fixed timestep game logic updates
		while (accumulator >= FRAME_TIME) {
			// Game logic simulation (runs at fixed rate)
			if (snake.length < 10) {
				snake.length++;
				state.snake = snake;
			}
			
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