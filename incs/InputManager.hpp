#pragma once

#include "DataStructs.hpp"
#include "raylib.h"
#include "Input.hpp"
#include <functional>
#include <map>

enum class InputContext {
	Menu,
	Gameplay,
	Paused,
	GameOver
};

enum class NavigationAction {
	Up,
	Down,
	Left,
	Right,
	Confirm,
	Cancel,
	Pause
};

struct MouseState {
	Vector2		position;
	bool		leftClicked;
	bool		rightClicked;
	Vector2		delta;
};

class InputManager {
	private:
		InputContext	currentContext;
		MouseState		mouseState;

		// key mappings
		std::map<NavigationAction, int> keyboardMappings;
		std::map<NavigationAction, int> gamepadMappings;

		// input callbacks
		std::function<void(NavigationAction)> onNavigation;
		std::function<void(Vector2, bool)> onMouseInput;

		// helpers
		void updateMouseState();
		bool isNavigationKeyPressed(NavigationAction action);

	public:
		InputManager();
		~InputManager() = default;

		void update();

		// callback registration
		void registerNavigationCallback(std::function<void(NavigationAction)> callback);
		void registerMouseCallback(std::function<void(Vector2, bool)> callback);

		// setters getters
		void setContext(InputContext context);
		InputContext getContext() const { return currentContext; }
		MouseState getMouseState() const { return mouseState; }

		// direct input polling queries and input management
		Input pollGameplayInput();
		void processInput(const Input input, GameState& state);

		// conf
		void remapKey(NavigationAction action, int key);
};