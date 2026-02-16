#include "../incs/Renderer.hpp"
#include "../incs/RaylibColors.hpp"
#include "../incs/ParticleSystem.hpp"
#include "../incs/TextSystem.hpp"
#include "../incs/AnimationSystem.hpp"
#include "../incs/MenuSystem.hpp"
#include "../incs/PostProcessingSystem.hpp"
#include "../incs/Snake.hpp"
#include "../incs/SnakeAI.hpp"
#include "../incs/Food.hpp"
#include "../incs/DataStructs.hpp"
#include "../incs/GameController.hpp"
#include "../incs/InputManager.hpp"
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

	// ENTITIES
	Snake snake_A(width, height);
	Snake snake_B(snake_A, width, height);
	Food food(Vec2{0, 0}, width, height);

	// CONFIGURATION AND STATE
	GameState state;
	state.width = width;
	state.height = height;
	state.snake_A = &snake_A;
	state.snake_B = &snake_B;
	state.food = &food;
	state.gameOver = false;
	state.isRunning = true;
	state.isPaused = false;
	state.currentState = GameStateType::Menu;
	state.score = 0;
	state.scoreB = 0;
	state.config.mode = GameMode::SINGLE;
	state.renderMode = RenderMode::MODE2D;
	state.timing.accumulator = 0.0f;
	state.aiController = nullptr;

	// SYSTEMS
	GameController gameController(&state);
	gameController.setAIController(nullptr);

	Renderer renderer;
	renderer.init(width, height);
	
	const int screenWidth = 1920;
	const int screenHeight = 1080;
	
	ParticleSystem particles(screenWidth, screenHeight, 10, 0, 30, 0.15f);
	
	TextSystem textSystem;
	textSystem.init();

	AnimationSystem animations;
	animations.init(1920, 1080);
	animations.enableTunnelEffect(true, TunnelConfig::menu());

	MenuSystem menu(gameController);
	menu.init(width, height);
	menu.setState(MenuState::Start);

	InputManager inputManager;
	inputManager.registerNavigationCallback([&menu](NavigationAction action) {
    	menu.handleNavigation(action);
	});
	inputManager.registerMouseCallback([&menu](Vector2 pos, bool clicked) {
		menu.handleMouseInput(pos, clicked);
	});

	PostProcessingSystem postProcess;
	postProcess.init(screenWidth, screenHeight);
	postProcess.setConfig(PostProcessingSystem::presetCRTBloom());

	// TIMING and preparations
	food.replaceInFreeSpace(&state);

	const double TARGET_FPS = 10.0;					// Snake moves 10 times per second
	const double FRAME_TIME = 1.0 / TARGET_FPS; 	// 0.1 seconds per update
	
	auto lastTime = std::chrono::high_resolution_clock::now();

	// MAIN GAME LOOP
	bool gameOverStateInitialized = false;
	
	while (state.isRunning) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameTime = currentTime - lastTime;
		float deltaTime = frameTime.count();
		lastTime = currentTime;
		
		// update phase
		inputManager.update();

		// general, cross modes poll input
		Input preInput = inputManager.pollGameplayInput();
		if (preInput == Input::ToggleFS) {
			ToggleFullscreen();
		}
		
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
				
				if (input == Input::Pause)
					inputManager.processInput(input, state);
				else if (input == Input::Switch2D || input == Input::Switch3D)
					inputManager.processInput(input, state);
			
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
				
			case GameStateType::Paused: {
				inputManager.setContext(InputContext::Paused);
            	Input input = inputManager.pollGameplayInput();
				
				if (input == Input::Pause)
					inputManager.processInput(input, state);
			}
				
			case GameStateType::GameOver: {
				if (!gameOverStateInitialized) {
					menu.setState(MenuState::GameOver);
					gameOverStateInitialized = true;
				}
				
				inputManager.setContext(InputContext::GameOver);
				break;
			}
		}
		
		//rendering phase
		postProcess.beginCapture();
		ClearBackground(Color{23, 23, 23, 255});

		// Update particles
		particles.update(deltaTime);
				
		// Update animations (tunnel effect)
		animations.updateTunnelEffect(deltaTime);
		
		switch (state.currentState) {
			case GameStateType::Menu: {
				BeginMode2D(renderer.getCamera2D());
				menu.render(renderer, textSystem, particles, animations, state);
				EndMode2D();
				break;
			}

			case GameStateType::Playing:
			case GameStateType::Paused: {
				switch (state.renderMode) {
					case RenderMode::MODE3D:
						// 3D gameplay rendering (Paused uses same render, just frozen)
						BeginMode3D(renderer.getCamera3D());
						renderer.render3D(state, state.isPaused ? 0.0f : deltaTime);
						EndMode3D();
						
						// UI overlay
						/* DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, customWhite);
						DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, customWhite);
						DrawFPS(screenWidth - 95, 10); */
						break;

					case RenderMode::MODE2D:
						// Update renderer state
						BeginMode2D(renderer.getCamera2D());
						renderer.render2D(state, state.isPaused ? 0.0f : deltaTime, particles, animations, snakeALightTop);
						//renderer.drawSnake2D(state.snake_A);
						//renderer.drawFood2D(state.food);
						EndMode2D();
						break;
				}
				
				if (state.isPaused) {
					DrawText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customBlack);
				}
				break;
			}			
		
			case GameStateType::GameOver: {
				BeginMode2D((Camera2D){(Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 0.0f, 1.0f});
				menu.renderGameOver(renderer, textSystem, particles, animations, state);
				EndMode2D();
				break;
			}
		}
		
		postProcess.endCapture();
		
		// Apply postprocessing and present to screen
		BeginDrawing();
		ClearBackground(BLACK);
		postProcess.applyAndPresent(deltaTime);
		EndDrawing();
	}
	
	return 0;
}