#include "../../incs/GridHelper.hpp"

int GridHelper::manhattanDistance(Vec2 a, Vec2 b) {
	return (abs(a.x - b.x) + abs(a.y - b.y));
}

bool GridHelper::isWalkable(const GameState& state, Vec2 pos,
							const std::vector<Vec2>& ignorePositions) {
	int x = pos.x;
	int y = pos.y;

	// Bounds check
	if (x < 0 || y < 0 || x >= state.width || y >= state.height)
		return false;

	// Check if in ignore list first
	for (const Vec2& ignored : ignorePositions) {
		if (pos.x == ignored.x && pos.y == ignored.y)
			return true;  // ✅ Treat as walkable
	}

	// Check snake a collision
	const Vec2* snakeA_body = state.snake_A->getSegments();
	int snakeA_len = state.snake_A->getLength();
	
	for (int i = 0; i < snakeA_len; i++) {
		if (snakeA_body[i].x == x && snakeA_body[i].y == y)
			return false;
	}

	// Check snake b collision
	if (state.snake_B) {
		const Vec2* snakeB_body = state.snake_B->getSegments();
		int snakeB_len = state.snake_B->getLength();
		
		for (int i = 0; i < snakeB_len; i++) {
			if (snakeB_body[i].x == x && snakeB_body[i].y == y)
				return false;
		}
	}

	return true;
}

std::vector<Vec2> GridHelper::getNeighbors(Vec2 pos) {
	std::vector<Vec2> neighbors;

	int x = pos.x;
	int y = pos.y;

	neighbors.push_back({x - 1, y});	//left
	neighbors.push_back({x + 1, y});	//right
	neighbors.push_back({x, y - 1});	//up
	neighbors.push_back({x, y + 1});	//down

	return neighbors;
}