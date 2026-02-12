#include "../../incs/MenuSystem.hpp"
#include "../../incs/Renderer.hpp"
#include "../../incs/ParticleSystem.hpp"
#include "../../incs/TextSystem.hpp"
#include "../../incs/AnimationSystem.hpp"

MenuSystem::MenuSystem(GameController &controller) :
	gameController(controller),
	currentState(MenuState::Start),
	particleSpawnTimer(0.0f),
	logoSnakeTrailCounter(0),
	screenWidth(1920),
	screenHeight(1080) {}

void MenuSystem::init(int width, int height) {
	(void)width;   // Reserved for future use
	(void)height;  // Reserved for future use
	screenWidth = 1920;
	screenHeight = 1080;
	setState(MenuState::Start);
}

void MenuSystem::setState(MenuState newState) {
	currentState = newState;
	clearButtons();
	initializeButtons();
}

void MenuSystem::initializeButtons() {
	buttons.clear();

	// Unified while coding the buttons, will design them properly later.
	// Colors
		Color buttonNormal = {70, 130, 180, 200};   // Semi-transparent blue
		Color buttonHover = {100, 160, 210, 255};   // Brighter blue
		Color textColor = {255, 248, 227, 255};     // White
		
		// Button dimensions
		float buttonWidth = 300;
		float buttonHeight = 60;
		float spacing = 20;
		
		// Center buttons below the logo
		float startY = screenHeight / 2 + 200;
		float centerX = screenWidth / 2 - buttonWidth / 2;
	
	if (currentState == MenuState::Start) {		
		// Start button (placeholder for now)
		Button startButton;
		startButton.bounds = {centerX, startY, buttonWidth, buttonHeight};
		startButton.Text = "Start Game";
		startButton.normalColor = buttonNormal;
		startButton.hoverColor = buttonHover;
		startButton.textColor = textColor;
		startButton.onClick = [this]() {
			this->startGame();
		};
		
		buttons.push_back(startButton);
		
		// Mode toggle button (shows current mode)
		Button modeButton;
		modeButton.bounds = {centerX, startY + buttonHeight + spacing, buttonWidth, buttonHeight};
		modeButton.Text = "Change mode";
		modeButton.normalColor = buttonNormal;
		modeButton.hoverColor = buttonHover;
		modeButton.textColor = textColor;
		modeButton.onClick = [this]() { 
			this->switchConfigMode();
		};
		
		buttons.push_back(modeButton);
	} else if (currentState == MenuState::GameOver) {
		// Restart button
		Button restartButton;
		restartButton.bounds = {centerX, startY + buttonHeight + spacing, buttonWidth, buttonHeight};
		restartButton.Text = "Main Menu";
		restartButton.normalColor = buttonNormal;
		restartButton.hoverColor = buttonHover;
		restartButton.textColor = textColor;
		restartButton.onClick = [this] () {
			this->restartGame();
		};

		buttons.push_back(restartButton);
	} // Other menu states coming soon
}

void MenuSystem::clearButtons() {
	buttons.clear();
}

void MenuSystem::spawnMenuParticles(float deltaTime, ParticleSystem& particles) {
	// Logo snake trail particle spawning (from renderMenu)
	particleSpawnTimer += deltaTime;
	logoSnakeTrailCounter++;
	
	if (logoSnakeTrailCounter % 5 == 0) {
		int square = 10;
		float trailX = screenWidth / 2.0f + (square * 17.2f);
		float trailY = screenHeight / 2.0f + (square * 3.2f);
		Color lightBlue = {70, 130, 180, 255};
		particles.spawnSnakeTrail(trailX, trailY, 1, 0, lightBlue);
	}
}

void MenuSystem::update(float deltaTime, ParticleSystem& particles, AnimationSystem& animations) {
	// Spawn menu-specific particles
	spawnMenuParticles(deltaTime, particles);
	
	// Update particles
	particles.update(deltaTime);
	
	// Update animations (tunnel effect)
	animations.updateTunnelEffect(deltaTime);
}

void MenuSystem::render(Renderer& renderer, TextSystem& textSystem,
						ParticleSystem& particles, AnimationSystem& animations,
						const GameState& state) {
	int screenCenterX = screenWidth / 2;
	int screenCenterY = screenHeight / 2;
	int square = 10;
	int sep = 15;
	
	// Render logo
	textSystem.drawLogo(screenCenterX, screenCenterY, square, sep, 
					Color{255, 248, 227, 255},  // customWhite
					Color{70, 130, 180, 255},   // blue
					Color{254, 74, 81, 255});   // red
	
	// Show current game mode below logo
	const char* modeText = "";
	switch (state.config.mode) {
		case GameMode::SINGLE:
			modeText = "Mode: SINGLE PLAYER";
			break;
		case GameMode::MULTI:
			modeText = "Mode: MULTIPLAYER";
			break;
		case GameMode::AI:
			modeText = "Mode: VS AI";
			break;
	}
	int modeTextWidth = MeasureText(modeText, 30);
	DrawText(modeText, screenCenterX - modeTextWidth / 2, screenCenterY + 100, 30, Color{255, 248, 227, 255});
	
	// Render buttons
	Vector2 mousePos = GetMousePosition();
	for (const auto& button : buttons) {
		bool hovered = button.isHovered(mousePos);
		button.render(hovered);
	}
	
	// Render particles
	particles.render();
	
	// Render tunnel effect
	animations.renderTunnelEffect();
	
	// Render border
	renderer.drawBorder(25);
}

void MenuSystem::renderGameOver(Renderer& renderer, TextSystem& textSystem,
								ParticleSystem& particles, AnimationSystem& animations,
								const GameState& state) {
	int screenCenterX = screenWidth / 2;
	int screenCenterY = screenHeight / 2;
	int square = 10;
	int sep = 15;
	
	// Render particles
	particles.render();
	
	// Render game over logo
	textSystem.drawGameOverLogo(screenCenterX, screenCenterY, square, sep,
							Color{255, 248, 227, 255},  // customWhite
							Color{125, 125, 125, 255}); // customGray
	
	// Show winner if not single player
	if (state.config.mode != GameMode::SINGLE) {
		textSystem.drawWinner(state, screenCenterX, screenCenterY, Color{255, 248, 227, 255});
	}
	
	// Show scores and retry prompt
	//textSystem.drawScore(state, screenCenterX, screenCenterY, Color{255, 248, 227, 255});
	//textSystem.drawRetryPrompt(screenCenterX, screenCenterY, Color{255, 248, 227, 255});

	Vector2 mousePos = GetMousePosition();
	for (const auto& button : buttons) {
		bool hovered = button.isHovered(mousePos);
		button.render(hovered);
	}
	
	// Render tunnel effect
	animations.renderTunnelEffect();
	
	// Render border
	renderer.drawBorder(25);
}

void MenuSystem::handleInput(Vector2 mousePos, bool mouseClicked) {
	if (mouseClicked) {
		for (auto& button : buttons) {
			if (button.isHovered(mousePos)) {
				if (button.onClick) {
					button.onClick();
				}
				break;
			}
		}
	}
}

Button* MenuSystem::getHoveredButton(Vector2 mousePos) const {
	for (const auto& button : buttons) {
		if (button.isHovered(mousePos)) {
			return const_cast<Button*>(&button);
		}
	}
	return nullptr;
}

// button functions

void MenuSystem::startGame() {
	GameState &state = gameController.getState();

	state.timing.accumulator = 0.0f;
	state.timing.lastFrameTime = 0.0f;
	
	state.aiController.reset();

	if (state.aiController) {
		gameController.setAIController(nullptr);
	}

if (state.config.mode == GameMode::AI) {
		state.aiController = std::make_unique<SnakeAI>(AIConfig::medium());
	}
	
	state.currentState = GameStateType::Playing;
	
}

void MenuSystem::switchConfigMode()
{
	GameConfig &config = gameController.getState().config;
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
	
	// Refresh buttons
	initializeButtons();
}

void MenuSystem::restartGame() {
	GameState &state = gameController.getState();

	state.snake_A->reset(state.width, state.height);
	state.snake_B->resetAsMirrored(*state.snake_A, state.width, state.height);  // Mirror snake_A
	state.food->reset(&state);
	state.score = 0;
	state.scoreB = 0;
	state.snake_A->setAsDead(false);
	state.snake_B->setAsDead(false);
	state.gameOver = false;
	state.isPaused = false;
	state.timing.accumulator = 0.0;
	gameController.clearInputBuffer();
	state.currentState = GameStateType::Menu;
	setState(MenuState::Start);
	// Note: gameOverStateSet flag in main.cpp will be reset on next state transition
}
