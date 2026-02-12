#include "../incs/Renderer.hpp"
#include "../incs/ParticleSystem.hpp"
#include "../incs/TextSystem.hpp"
#include "../incs/AnimationSystem.hpp"
#include "../incs/MenuSystem.hpp"
#include "../incs/Snake.hpp"
#include "../incs/SnakeAI.hpp"
#include "../incs/Food.hpp"
#include "../incs/DataStructs.hpp"
#include "../incs/GameController.hpp"
#include "../incs/Utils.hpp"
#include "../incs/colors.h"
#include <thread>
#include <iostream>
#include <memory>

bool parseArguments(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		std::string str(argv[i]);
		if (str.find_first_not_of("0123456789") != std::string::npos) {
			std::cerr << "error: bad argument {" << argv[i] << "}: only numeric arguments accepted" << std::endl;
			return false;
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
	if (argc != 3)
	{
		std::cerr << BYEL << "Usage: ./rosario <width> <height>" << RESET << std::endl;
		return 1;
	}

	if (!parseArguments(argc, argv))
	{
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);

	if (width < 16 || height < 16 || width > 41 || height > 41)
	{
		std::cerr << "Minimal arena width and height values are 16 units! Try running again with those or higher values!" << std::endl;
		return 1;
	}


	// SYSTEMS
	Renderer renderer;
	renderer.init(width, height);
	
	ParticleSystem particles(1920, 1080, 10, 0, 30, 0.15f);
	
	TextSystem textSystem;
	textSystem.init();

	AnimationSystem animations;
	animations.init(1920, 1080);
	animations.enableTunnelEffect(true, TunnelConfig::menu());

	MenuSystem menu;
	menu.init(width, height);

	// ENTITIES
	Snake snake_A(width, height);
	Snake snake_B(snake_A, width, height);
	std::unique_ptr<SnakeAI> aiController = nullptr;

	Food food(Vec2{0, 0}, width, height);

	// CONFIGURATION AND STATE
	GameConfig config { GameMode::SINGLE };
	
	GameState state {
		width, height, snake_A, &snake_B, food,
		false,
		true,
		false,
		GameStateType::Menu,
		0,
		0,
		config
	};
	
	food.replaceInFreeSpace(&state);

	GameController gameController(&state);

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
		
		Input input = renderer.pollInput();
		
		if (input == Input::Quit) {
			state.isRunning = false;
			break;
		}
		
		// STATE MACHINE
		switch (state.currentState) {
			case GameStateType::Menu:
				if (input == Input::Enter) {
					if (aiController) {
						gameController.setAIController(nullptr);
						aiController.reset();
					}
					
					if (state.config.mode == GameMode::AI) {
						aiController = std::make_unique<SnakeAI>(AIConfig::medium());
						gameController.setAIController(aiController.get());
					}
					
					state.currentState = GameStateType::Playing;
					accumulator = 0.0;
				} else if (input == Input::Pause) {
					switchConfigMode(state.config);
				}
				
				menu.update(deltaTime, particles, animations);
				
				BeginDrawing();
				ClearBackground(Color{23, 23, 23, 255});
				BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
				menu.render(renderer, textSystem, particles, animations, state);
				EndMode2D();
				EndDrawing();
				break;			case GameStateType::Playing:
				if (input == Input::Pause) {
					state.isPaused = !state.isPaused;
					state.currentState = state.isPaused ? 
						GameStateType::Paused : GameStateType::Playing;
				}
				
				accumulator += deltaTime;
				gameController.bufferInput(input);
				
				while (accumulator >= FRAME_TIME) {
					gameController.update();
					accumulator -= FRAME_TIME;
					
					if (!state.isRunning) {
						state.currentState = GameStateType::GameOver;
						state.isRunning = true;
						break;
					}
				}
				
				renderer.render(state, deltaTime);
				break;
				
			case GameStateType::Paused:
				if (input == Input::Pause) {
					state.isPaused = false;
					state.currentState = GameStateType::Playing;
				}
				renderer.render(state, 0.0f);
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
					gameController.clearInputBuffer();
					state.currentState = GameStateType::Menu;

					// Update and render menu
					menu.update(deltaTime, particles, animations);
					BeginDrawing();
					ClearBackground(Color{23, 23, 23, 255});
					BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
					menu.render(renderer, textSystem, particles, animations, state);
					EndMode2D();
					EndDrawing();
				} else {
					// Update game over menu
					particles.update(deltaTime);
					animations.updateTunnelEffect(deltaTime);
					
					BeginDrawing();
					ClearBackground(Color{23, 23, 23, 255});
					BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
					menu.renderGameOver(renderer, textSystem, particles, animations, state);
					EndMode2D();
					// renderer.drawNoiseGrain(); // keeping noise for contrast in game over
					EndDrawing();
				}
				break;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	return 0;
}