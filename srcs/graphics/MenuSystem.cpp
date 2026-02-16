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
		Color buttonOutline = snakeALightSide;   // Semi-transparent blue
		Color buttonBackground = customBlack;
		Color textColor = customWhite;     // White
		Color textHoverColor = customBlack;
		Color buttonHover = customWhite;   // Brighter blue
		Color outlineHoverColor = customWhite;
		
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
		startButton.outlineColor = buttonOutline;
		startButton.backgroundColor = buttonBackground;
		startButton.hoverColor = buttonHover;
		startButton.textColor = textColor;
		startButton.textHoverColor = textHoverColor;
		startButton.outlineHoverColor = outlineHoverColor;
		startButton.onClick = [this]() {
			this->startGame();
		};
		
		buttons.push_back(startButton);
		
		// Mode toggle button (shows current mode)
		Button modeButton;
		modeButton.bounds = {centerX, startY + buttonHeight + spacing, buttonWidth, buttonHeight};
		modeButton.Text = "Change mode";
		modeButton.outlineColor = buttonOutline;
		modeButton.backgroundColor = buttonBackground;
		modeButton.hoverColor = buttonHover;
		modeButton.textColor = textColor;
		modeButton.textHoverColor = textHoverColor;
		modeButton.outlineHoverColor = outlineHoverColor;
		modeButton.onClick = [this]() { 
			this->switchConfigMode();
		};
		
		buttons.push_back(modeButton);

		// Quit button
		Button quitButton;
		quitButton.bounds = {centerX, startY + (buttonHeight * 2) + (spacing * 2), buttonWidth, buttonHeight};
		quitButton.Text = "Quit";
		quitButton.outlineColor = buttonOutline;
		quitButton.backgroundColor = buttonBackground;
		quitButton.hoverColor = buttonHover;
		quitButton.textColor = textColor;
		quitButton.textHoverColor = textHoverColor;
		quitButton.outlineHoverColor = outlineHoverColor;
		quitButton.onClick = [this]() { 
			this->quitGame();
		};
		
		buttons.push_back(quitButton);
	} else if (currentState == MenuState::GameOver) {
		// Restart button
		Button restartButton;
		restartButton.bounds = {centerX, startY, buttonWidth, buttonHeight};
		restartButton.Text = "Main Menu";
		restartButton.outlineColor = buttonOutline;
		restartButton.backgroundColor = buttonBackground;
		restartButton.hoverColor = buttonHover;
		restartButton.textColor = textColor;
		restartButton.textHoverColor = textHoverColor;
		restartButton.outlineHoverColor = outlineHoverColor;
		restartButton.onClick = [this] () {
			this->restartGame();
		};

		buttons.push_back(restartButton);

		// Quit button
		Button quitButton;
		quitButton.bounds = {centerX, startY + buttonHeight + spacing, buttonWidth, buttonHeight};
		quitButton.Text = "Quit";
		quitButton.outlineColor = buttonOutline;
		quitButton.backgroundColor = buttonBackground;
		quitButton.hoverColor = buttonHover;
		quitButton.textColor = textColor;
		quitButton.textHoverColor = textHoverColor;
		quitButton.outlineHoverColor = outlineHoverColor;
		quitButton.onClick = [this]() { 
			this->quitGame();
		};
		
		buttons.push_back(quitButton);
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
		int square = 30;
		float trailX = screenWidth / 2.0f + (square * 18.2f);
		float trailY = screenHeight / 2.0f + (square * 3.2f);
		Color lightBlue = {70, 130, 180, 255};
		particles.spawnSnakeTrail(trailX, trailY, 1, 0, lightBlue);
	}
}

void MenuSystem::update(float deltaTime, ParticleSystem& particles, AnimationSystem& animations) {
	// Spawn menu-specific particles
	spawnMenuParticles(deltaTime, particles);
}

void MenuSystem::render(Renderer& renderer, TextSystem& textSystem,
						ParticleSystem& particles, AnimationSystem& animations,
						const GameState& state) {
	int screenCenterX = screenWidth / 2;
	int screenCenterY = screenHeight / 2;
	int square = 30;
	int sep = 15;

	// Render particles
	particles.render();
	
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
	DrawText(modeText, screenCenterX - modeTextWidth / 2, screenCenterY + 135, 30, Color{255, 248, 227, 255});

	// Render buttons
	Vector2 mousePos = GetMousePosition();
	for (size_t i = 0; i < buttons.size(); i++) {
		bool hovered = buttons[i].isHovered(mousePos) || (i == static_cast<size_t>(selectedButtonIndex));
		buttons[i].render(hovered);
	}
	
	// Render tunnel effect
	animations.renderTunnelEffect();
	
	// Render fullscreen border
	renderer.drawBorderFullscreen(25);
}

void MenuSystem::renderGameOver(Renderer& renderer, TextSystem& textSystem,
								ParticleSystem& particles, AnimationSystem& animations,
								const GameState& state) {
	int screenCenterX = screenWidth / 2;
	int screenCenterY = screenHeight / 2;
	int square = 30;
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
		for (size_t i = 0; i < buttons.size(); i++) {
			bool hovered = buttons[i].isHovered(mousePos) || (i == static_cast<size_t>(selectedButtonIndex));
			buttons[i].render(hovered);
		}
	
	// Render tunnel effect
	animations.renderTunnelEffect();
	
	// Render fullscreen border
	renderer.drawBorderFullscreen(25);
}

// input related methods
Button* MenuSystem::getHoveredButton(Vector2 mousePos) const {
	for (const auto& button : buttons) {
		if (button.isHovered(mousePos)) {
			return const_cast<Button*>(&button);
		}
	}
	return nullptr;
}

void MenuSystem::handleNavigation(NavigationAction action) {
	switch (action) {
		case NavigationAction::Up:
			selectPreviousButton();
			break;

		case NavigationAction::Down:
			selectNextButton();
			break;

		case NavigationAction::Confirm:
			// Activate selected button
			if (selectedButtonIndex >= 0 && static_cast<size_t>(selectedButtonIndex) < buttons.size()) {
				if (buttons[selectedButtonIndex].onClick) {
					buttons[selectedButtonIndex].onClick();
				}
			}
			break;

		case NavigationAction::Cancel:
			// Go back / quite
			if (currentState == MenuState::Start) {
				quitGame();
			}
			break;

		default:
			break;
	}
}

void MenuSystem::handleMouseInput(Vector2 mousePos, bool mouseClicked) {
	for (size_t i = 0; i < buttons.size(); i++) {
		if (buttons[i].isHovered(mousePos)) {
			selectedButtonIndex =  i;

			if (mouseClicked && buttons[i].onClick) {
				buttons[i].onClick();
			}
			break;
		}
	}
}

void MenuSystem::selectNextButton() {
	if (static_cast<size_t>(selectedButtonIndex) > buttons.size() - 1)
		selectedButtonIndex = 0;
	else
		selectedButtonIndex = (selectedButtonIndex + 1) % buttons.size();
}

void MenuSystem::selectPreviousButton() {
	if (static_cast<size_t>(selectedButtonIndex) > buttons.size() - 1)
		selectedButtonIndex = 0;
	else {
		selectedButtonIndex--;
		if (selectedButtonIndex < 0) {
			selectedButtonIndex = buttons.size() - 1;
		}
	}
}

// button functions

void MenuSystem::startGame() {
	GameState &state = gameController.getState();

	// Reset timing
	state.timing.accumulator = 0.0f;
	state.timing.lastFrameTime = 0.0f;
	
	// Clear old AI (if any)
	state.aiController.reset();
	gameController.setAIController(nullptr);
	
	// Create new AI if needed
	if (state.config.mode == GameMode::AI) {
		state.aiController = std::make_unique<SnakeAI>(AIConfig::medium());
		gameController.setAIController(state.aiController.get()); 
	}
	
	state.currentState = GameStateType::Playing;
	selectedButtonIndex = 99;
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

	resetButtonIndex();

	setState(MenuState::Start);
	// gameOverStateSet flag in main.cpp will be reset on next state transition
}

void MenuSystem::quitGame() {
	gameController.getState().isRunning = false;
}
