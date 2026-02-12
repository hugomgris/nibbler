#pragma once
#include "DataStructs.hpp"
#include "Input.hpp"
#include "Snake.hpp"
#include "SnakeAI.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include <iostream>
#include <chrono>
#include <queue>

class GameController {
	private:
		GameState*			_state;
		std::queue<Input>	inputBuffer_A;
		std::queue<Input>	inputBuffer_B;
		static const size_t	MAX_BUFFER_SIZE = 3;

		SnakeAI *aiController;
		int aiThinkCounter;

		using time = std::chrono::time_point<std::chrono::high_resolution_clock>;

		void processNextInput();

	public:
		GameController();
		GameController(GameState *state);
		GameController(const GameController &other) = delete;
		GameController &operator=(const GameController &other) = delete;

		~GameController() = default;

		void update();

		GameState& getState() const { return *_state; }
		void setAIController(SnakeAI *ai);

		void bufferInput(Input input);
		void clearInputBuffer();

		void checkHeadFoodCollision();
		bool checkGameOverCollision();
};
