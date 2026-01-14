#pragma once
#include "GameState.hpp"
#include "Input.hpp"
#include "Snake.hpp"
#include <iostream>
#include <chrono>

class GameManager {
	private:
		Snake *_snake;
		using time = std::chrono::time_point<std::chrono::high_resolution_clock>;

	public:
		GameManager(Snake *snake);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default;

		void update();
		void calculateDeltaTime(time *lastTime, double* accumulator);
		bool handleGameInput(Input input);
};