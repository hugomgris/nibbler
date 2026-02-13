#pragma once

#include "Button.hpp"
#include "DataStructs.hpp"
#include "GameController.hpp"
#include "InputManager.hpp"
#include <raylib.h>
#include <vector>

class Renderer;
class ParticleSystem;
class TextSystem;
class AnimationSystem;

enum class MenuState {
	Start,
	Paused,
	GameOver,
	Options
};

class MenuSystem {
	private:
		GameController		&gameController;
		MenuState			currentState;
		std::vector<Button>	buttons;
		int					selectedButtonIndex;

		// menu specific particle states
		float				particleSpawnTimer;
		const float			particleSpawnInterval = 0.15f;
		int					logoSnakeTrailCounter;

		// cached screen dimensions
		int					screenWidth;
		int					screenHeight;

		//helpers
		void				spawnMenuParticles(float deltatime, ParticleSystem& particles);
		void				initializeButtons();
		void				clearButtons();

	
	public:
		MenuSystem(GameController &gameController);;
		~MenuSystem() = default;

		void init(int width, int height);

		// getters setters
		void setState(MenuState newStat);
		MenuState getstate() const { return currentState; }
		void resetButtonIndex() { selectedButtonIndex = 99; }
		int getButtonIndex() { return selectedButtonIndex; }
		int getButtonAmount() { return buttons.size() - 1; }

		// update and render for each menu state
		void update(float deltaTime, ParticleSystem& particles, AnimationSystem& animations);
		void render(Renderer &renderer, TextSystem& textSystem,
					ParticleSystem& particles, AnimationSystem& animations,
					const GameState& state);
		void renderGameOver(Renderer &renderer, TextSystem& textSystem,
							ParticleSystem& particles, AnimationSystem& animations,
							const GameState& state);        // input handling
		//void handleInput(Vector2 mousePos, bool mouseClicked);
		// input related methods
		Button* getHoveredButton(Vector2 mousePos) const;

		void handleMouseInput(Vector2 mousePos, bool mouseClicked);
		void handleNavigation(NavigationAction action);

		void selectButton(int index);
		void selectNextButton();
		void selectPreviousButton();

		// button functions
		void startGame();
		void switchConfigMode();
		void restartGame();
		void quitGame();
};