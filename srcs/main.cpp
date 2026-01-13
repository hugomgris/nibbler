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