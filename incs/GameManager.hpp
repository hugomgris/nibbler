#pragma once
#include "GameState.hpp"
#include "Snake.hpp"
#include <iostream>

class GameManager {
	private:
		Snake *_snake;

	public:
		GameManager(Snake *snake);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default; // TODO: ?

		void moveSnake();
};