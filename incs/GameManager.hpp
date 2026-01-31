#pragma once
#include "DataStructs.hpp"
#include "Input.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include <iostream>
#include <chrono>
#include <queue>

class GameManager {
	private:
		GameState*			_state;
		std::queue<Input>	inputBuffer;
		static const size_t	MAX_BUFFER_SIZE = 3;

		using time = std::chrono::time_point<std::chrono::high_resolution_clock>;

		void processNextInput();

	public:
		GameManager(GameState *state);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default;

		void update();

		void bufferInput(Input input);
		void clearInputBuffer();

		void checkHeadFoodCollision();
		bool checkGameOverCollision();
};
