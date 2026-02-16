#include "../../incs/InputManager.hpp"
#include <iostream>

InputManager::InputManager() : currentContext(InputContext::Menu) {
	keyboardMappings[NavigationAction::Up] = KEY_UP;
	keyboardMappings[NavigationAction::Down] = KEY_DOWN;
	keyboardMappings[NavigationAction::Left] = KEY_LEFT;
	keyboardMappings[NavigationAction::Right] = KEY_RIGHT;
	keyboardMappings[NavigationAction::Confirm] = KEY_ENTER;
	keyboardMappings[NavigationAction::Cancel] = KEY_ESCAPE;
	keyboardMappings[NavigationAction::Pause] = KEY_P;
}

void InputManager::update() {
	updateMouseState();

	// input procesing is context based
	switch (currentContext) {
		case InputContext::Menu:
		case InputContext::GameOver:
			// check for navigation actions
			for (auto& [action, key] : keyboardMappings) {
				if (IsKeyPressed(key)) {
					if (onNavigation) {
						onNavigation(action);
					}
				}
			}

			// Mouse input
			if (onMouseInput) {
				onMouseInput(mouseState.position, mouseState.leftClicked);
			}
			break;

		case InputContext::Gameplay:
			// game specific input is handled separatedly
			break;

		case InputContext::Paused:
			// for now, only unpausing is managed
			if (IsKeyPressed(keyboardMappings[NavigationAction::Pause])) {
				if (onNavigation) {
					onNavigation(NavigationAction::Pause);
				}
			}
			break;
	}
}

void InputManager::updateMouseState() {
	mouseState.position = GetMousePosition();
	mouseState.leftClicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	mouseState.rightClicked = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
	mouseState.delta = GetMouseDelta();
}

Input InputManager::pollGameplayInput() {
	if (IsKeyPressed(KEY_UP))		return Input::Up_A;
	if (IsKeyPressed(KEY_DOWN))		return Input::Down_A;
	if (IsKeyPressed(KEY_LEFT))		return Input::Left_A;
	if (IsKeyPressed(KEY_RIGHT))	return Input::Right_A;
	if (IsKeyPressed(KEY_W))		return Input::Up_B;
	if (IsKeyPressed(KEY_S))		return Input::Down_B;
	if (IsKeyPressed(KEY_A))		return Input::Left_B;
	if (IsKeyPressed(KEY_D))		return Input::Right_B;
	if (IsKeyPressed(KEY_Q))		return Input::Quit;
	if (IsKeyPressed(KEY_ESCAPE))	return Input::Quit;
	if (IsKeyPressed(KEY_SPACE))	return Input::Pause;
	if (IsKeyPressed(KEY_ENTER))	return Input::Enter;
	if (IsKeyPressed(KEY_ONE))		return Input::Switch2D;
	if (IsKeyPressed(KEY_TWO))		return Input::Switch3D;
	if (IsKeyPressed(KEY_KP_ENTER))	return Input::ToggleFS;
	
	if (WindowShouldClose())		return Input::Quit;
	
	return Input::None;
}

void InputManager::processInput(const Input input, GameState& state) {
	if (input == Input::Pause &&
		(state.currentState == GameStateType::Playing || state.currentState == GameStateType::Paused)) {
		state.isPaused = !state.isPaused;
		state.currentState = state.isPaused ? GameStateType::Paused : GameStateType::Playing;
		return;
	}

	RenderMode renderMode = state.renderMode;
	std::cout << "rendermode:" << static_cast<int>(renderMode) << std::endl;
	if (input == Input::Switch2D && renderMode == RenderMode::MODE3D) {
		state.renderMode = RenderMode::MODE2D;
		std::cout << "SWITCHING TO 2D MODE" << std::endl;
	}
	else if (input == Input::Switch3D && renderMode == RenderMode::MODE2D)
	{
		state.renderMode = RenderMode::MODE3D;
		std::cout << "SWITCHING TO 3D MODE" << std::endl;
	}
}

void InputManager::registerNavigationCallback(std::function<void(NavigationAction)> callback) {
	onNavigation = callback;
}

void InputManager::registerMouseCallback(std::function<void(Vector2, bool)> callback) {
	onMouseInput = callback;
}

void InputManager::setContext(InputContext context) {
	currentContext = context;
}
