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
#include <ncurses.h>
#include <array>
#include <string_view>

// Cleanup handler for ncurses when program exits
void cleanupNCurses() {
	if (isendwin() == FALSE) {
		endwin();
		std::cout << BYEL << "[Main] Called endwin() on exit" << RESET << std::endl;
	}
}

int main(int argc, char **argv) {
	std::atexit(cleanupNCurses); // This might not be necessary after switching to an external, dynamically linked Ncurses, but we'll leave it just in case (legacy!)
	
	if (argc != 3)
	{
		std::cerr << BYEL << "Usage: ./nibbler <width> <height>" << RESET << std::endl;
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);

	if (width < 16 || height < 16)
	{
		std::cerr << "Minimal arena width and height values are 16 units! Try running again with those or higher values!" << std::endl;
		return 1;
	}

	constexpr std::array<std::string_view, 3> libs = {
		"./nibbler_ncurses.so",
		"./nibbler_sdl.so",
		"./nibbler_raylib.so"
	};
	int currentLib = 1;

	LibraryManager gfxLib;
	if (!gfxLib.load(libs[currentLib].data()))
		return 1;

	gfxLib.get()->init(width, height);

	Snake snake(width, height);
	Food food(Utils::getRandomVec2(width - 1, height - 1), width, height);
	GameState state {
		width, height, snake, food,
		false,
		true,
		false,
		GameStateType::Menu,
		0
	};

	GameManager gameManager(&state);

	const double TARGET_FPS = 10.0;					// Snake moves 10 times per second
	const double FRAME_TIME = 1.0 / TARGET_FPS; 	// 0.1 seconds per update
	
	auto lastTime = std::chrono::high_resolution_clock::now();
	double accumulator = 0.0;

	// MAIN GAME LOOP
	while (state.isRunning) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameTime = currentTime - lastTime;
		float deltaTime = frameTime.count();
		lastTime = currentTime;
		
		Input input = gfxLib.get()->pollInput();
		
		if (input == Input::Quit) {
			state.isRunning = false;
			break;
		}
		
		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = (int)input - 1;
			if (newLib != currentLib) {
				gfxLib.unload();
				if (!gfxLib.load(libs[newLib].data())) return 1;
				gfxLib.get()->init(width, height);
				currentLib = newLib;
			}
		}
		
		// STATE MACHINE
		switch (state.currentState) {
			case GameStateType::Menu:
				if (input == Input::Enter) {
					state.currentState = GameStateType::Playing;
					accumulator = 0.0;
				}
				gfxLib.get()->renderMenu(state, deltaTime);
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
				gfxLib.get()->render(state, 0.0f);
				break;
				
			case GameStateType::GameOver:
				if (input == Input::Enter) {
					snake = Snake(width, height);
					food = Food(Utils::getRandomVec2(width - 1, height - 1), width, height);
					state.score = 0;
					state.gameOver = false;
					state.isPaused = false;
					accumulator = 0.0;
					gameManager.clearInputBuffer();
					
					state.currentState = GameStateType::Menu;
				}
				gfxLib.get()->renderGameOver(state, deltaTime);
				break;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	return 0;
}