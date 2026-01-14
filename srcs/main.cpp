#include "../incs/IGraphic.hpp"
#include "../incs/GameState.hpp"
#include "../incs/GameManager.hpp"
#include "../incs/LibraryManager.hpp"
#include "../incs/colors.h"
#include <chrono>  // for timing/FPS setup
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