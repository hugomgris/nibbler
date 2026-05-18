#include "../incs/IGraphic.hpp"
#include "../incs/IAudio.hpp"
#include "../incs/Snake.hpp"
#include "../incs/SnakeAI.hpp"
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

void printUsage() {
	std::cerr << BYEL << "Usage: ./nibbler <width> <height> <library" << RESET << std::endl;
	std::cerr << BBLU << "Supported libraries: ncurses, SDL, raylib" << std::endl;
}

bool parseArguments(int argc, char **argv, int *library)
{
	for (int i = 1; i < argc; i++) {
		std::string str(argv[i]);
		if (i < 3 && str.find_first_not_of("0123456789") != std::string::npos) {
			std::cerr << "error: bad argument {" << argv[i] << "}: only positive numeric arguments accepted for arena dimensions" << std::endl;
			return false;
		}

		if (i == 3)
		{
			if (str == "ncurses") {
				*library = 0;
			} else if (str == "SDL") {
				*library = 1;
			} else if (str == "raylib") {
				*library = 2;
			} else {
				std::cerr << "error: bad argument {" << argv[i] << "}: invalid library name" << std::endl;
				printUsage();
				return false;
			}
		}
	}

	return true;
}

void switchConfigMode(GameConfig &config)
{
	switch (config.mode)
	{
		case GameMode::SINGLE:
			config.mode = GameMode::MULTI;
			break;
		
		case GameMode::MULTI:
			config.mode = GameMode::AI;
			break;

		case GameMode::AI:
			config.mode = GameMode::SINGLE;
			break;
	}
}

int main(int argc, char **argv) {
	std::atexit(cleanupNCurses);
	
	if (argc == 2) {
		std::string str(argv[1]);
		if (str == "help") {
			printUsage();
			return 0;
		}
	}
	
	if (argc != 4)
	{
		printUsage();
		return 1;
	}

	int currentLib = -1;

	if (!parseArguments(argc, argv, &currentLib))
	{
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);

	if (width < 16 || height < 16 || width > 41 || height > 41)
	{
		std::cerr << "Error: Height and width values must be between 16 and 41" << std::endl;
		return 1;
	}

	GameConfig config { GameMode::SINGLE };

	constexpr std::array<std::string_view, 3> graphicLibs = {
		"./nibbler_ncurses.so",
		"./nibbler_sdl.so",
		"./nibbler_raylib.so"
	};

	const std::string audioLib = "./nibbler_sdl_mix.so";

	LibraryManager libManager;
	if (!libManager.loadGraphicLib(graphicLibs[currentLib].data()) || !libManager.loadAudioLib(audioLib.c_str()))
		return 1;

	// init graphics
	libManager.getGraphicLib()->init(width, height);

	//init audio
	libManager.getAudioLib()->init();

	Snake snake_A(width, height);
	Snake snake_B(snake_A, width, height);
	std::unique_ptr<SnakeAI> aiController = nullptr;

	Food food(Vec2{0, 0}, width, height);
	
	GameState state {
		width, height, snake_A, &snake_B, food,
		false,
		true,
		false,
		GameStateType::Menu,
		0,
		0,
		libManager.getAudioLib(),
		config
	};
	
	food.replaceInFreeSpace(&state);

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
		
		Input input = libManager.getGraphicLib()->pollInput();
		
		if (input == Input::Quit) {
			state.isRunning = false;
			break;
		}
		
		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = static_cast<int>(input) - 1;
			if (newLib != currentLib) {
				libManager.unloadGraphicLib();
				if (!libManager.loadGraphicLib(graphicLibs[newLib].data())) return 1;
				libManager.getGraphicLib()->init(width, height);

				if ((newLib == static_cast<int>(static_cast<int>(Input::SwitchLib2) - 1)) ||
						(currentLib == static_cast<int>(static_cast<int>(Input::SwitchLib2) - 1))) {
					libManager.unloadAudioLib();
					if (!libManager.loadAudioLib(audioLib.c_str())) return 1;
					libManager.getAudioLib()->init();
				}

				currentLib = newLib;
			}
		}
		
		// STATE MACHINE
		switch (state.currentState) {
			case GameStateType::Menu:
				if (input == Input::Enter) {
					if (aiController) {
						gameManager.setAIController(nullptr);
						aiController.reset();
					}
					
					if (state.config.mode == GameMode::AI) {
						aiController = std::make_unique<SnakeAI>(AIConfig::medium());
						gameManager.setAIController(aiController.get());
					}
					
					state.currentState = GameStateType::Playing;
					accumulator = 0.0;
			} else if (input == Input::Pause) {
				switchConfigMode(state.config);
			}
    
			libManager.getGraphicLib()->renderMenu(state, deltaTime);
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
				
				libManager.getGraphicLib()->render(state, deltaTime);
				break;
				
			case GameStateType::Paused:
				if (input == Input::Pause) {
					state.isPaused = false;
					state.currentState = GameStateType::Playing;
				}
				libManager.getGraphicLib()->render(state, 0.0f);
				break;
				
			case GameStateType::GameOver:
				if (input == Input::Enter) {
					snake_A = Snake(width, height);
					snake_B = Snake(snake_A, width, height);
					food = Food(Vec2{0, 0}, width, height);
					food.replaceInFreeSpace(&state);
					state.score = 0;
					state.scoreB = 0;
					state.snake_A.setAsDead(false);
					state.snake_B->setAsDead(false);
					state.gameOver = false;
					state.isPaused = false;
					accumulator = 0.0;
					gameManager.clearInputBuffer();
					state.currentState = GameStateType::Menu;

					libManager.getGraphicLib()->renderMenu(state, deltaTime); // I have to go straight into menu here to avoid weird behaviours in the raylib version
				} else {
					libManager.getGraphicLib()->renderGameOver(state, deltaTime);
				}
				break;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	return 0;
}