#pragma once

#include "../../incs/DataStructs.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"

namespace TestHelpers {
	inline GameState createBasicGameState(int width = 20, int height = 20) {
		static Snake snake(width, height);
		static Food food({10, 10}, width, height);
		static GameConfig config{GameMode::SINGLE};

		return GameState {
			width,
			height,
			&snake,
			nullptr,
			&food,
			false,					// gameover
			true,					// isRunning
			false,					// isPaused
			GameStateType::Playing,
			0,						// score
			0,						// scoreB
			config
		};
	}
}