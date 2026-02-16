#pragma once

#include "Snake.hpp"
#include "DataStructs.hpp"
#include "GridHelper.hpp"
#include <vector>
#include <queue>

class GridHelper;

class FloodFill : protected GridHelper {
public:
	FloodFill() = default;
	FloodFill(const FloodFill &other) = delete;
	FloodFill &operator=(const FloodFill &other) = delete;
	~FloodFill() = default;

	// count reachable empty cells from start
	int countReachable(
		const GameState& state,
		Vec2 start,
		const std::vector<Vec2>& ignorePositions = {}       // mostly for the tail right now
	);
	
	bool canReachTail(
		const GameState& state,
		const Snake* aiSnake,
		const std::vector<Vec2>& proposedPath
	);
};