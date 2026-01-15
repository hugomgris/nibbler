#pragma once
#include "DataStructs.hpp"
#include "Input.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include <iostream>
#include <chrono>

class GameManager {
	private:
		GameState	*_state;
		using time = std::chrono::time_point<std::chrono::high_resolution_clock>;

	public:
		GameManager(GameState *state);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default;

		void update();
		void calculateDeltaTime(time *lastTime, double* accumulator);
		bool handleGameInput(Input input);
		void checkHeadFoodCollision();
};