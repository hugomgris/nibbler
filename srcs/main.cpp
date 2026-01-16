#include "../incs/IGraphic.hpp"
#include "../incs/Snake.hpp"
#include "../incs/Food.hpp"
#include "../incs/DataStructs.hpp"
#include "../incs/GameManager.hpp"
#include "../incs/LibraryManager.hpp"
#include "../incs/Utils.hpp"
#include "../incs/colors.h"
#include <thread>
#include <fcntl.h>
#include <iostream>

int main(int argc, char **argv) {
	if (argc != 3)
	{
		std::cout << BYEL << "Usage: ./nibbler <width> <height>" << RESET << std::endl;
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);

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

	Snake snake;
	Food food(Utils::getRandomVec2(width - 1, height - 1), width, height);
	GameState state { width, height, &snake, &food, NULL, false, true };

	GameManager gameManager(&state);

	const double TARGET_FPS = 10.0;					// Snake moves 10 times per second
	const double FRAME_TIME = 1.0 / TARGET_FPS; 	// 0.1 seconds per update
	
	auto lastTime = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;
	int frameCount = 0;

	// MAIN GAME LOOP
	while (state.isRunning) {
		gameManager.calculateDeltaTime(&lastTime, &accumulator);
		
		Input input = gfxLib.get()->pollInput();
		
		if (input == Input::Quit) {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			state.isRunning = false;
			break;
		}
		
		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = (int)input - 1;
			
			if (newLib != currentLib) {
				std::cout << BMAG << "\nSwitching from lib " << (currentLib + 1) 
						<< " to lib " << (newLib + 1) << RESET << std::endl;
				
				gfxLib.unload();
				
				if (!gfxLib.load(libs[newLib])) {
					std::cerr << BRED << "Failed to load new library!" << RESET << std::endl;
					return 1;
				}
				
				gfxLib.get()->init(width, height);
				currentLib = newLib;
			}
		}
		
		gameManager.bufferInput(input);
		
		while (accumulator >= FRAME_TIME) {
			gameManager.update();
			frameCount++;
			accumulator -= FRAME_TIME;
		}
		
		gfxLib.get()->render(state);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}