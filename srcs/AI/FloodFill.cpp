#include "../../incs/FloodFill.hpp"

int FloodFill::countReachable(const GameState& state, Vec2 start, 
							const std::vector<Vec2>& ignorePositions) {
	// Bounds check for start position
	if (start.x < 0 || start.y < 0 || start.x >= state.width || start.y >= state.height)
		return 0;
	
	std::vector<std::vector<bool>> visited(state.width, 
										std::vector<bool>(state.height, false));
	
	std::queue<Vec2> queue;
	queue.push(start);
	visited[start.x][start.y] = true;
	
	int count = 0;
	
	while (!queue.empty()) {
		Vec2 current = queue.front();
		queue.pop();
		count++;
		
		for (Vec2 neighbor : getNeighbors(current)) {
			// Pass ignorePositions to isWalkable
			if (isWalkable(state, neighbor, ignorePositions) && 
				!visited[neighbor.x][neighbor.y]) {
				visited[neighbor.x][neighbor.y] = true;
				queue.push(neighbor);
			}
		}
	}
	
	return count;
}

bool FloodFill::canReachTail(const GameState &state, const Snake *aiSnake, const std::vector<Vec2> &proposedPath) {
	if (proposedPath.empty())
		return false;

	// 1 get the new head position (i.e., end of path)
	Vec2 newHead = proposedPath[proposedPath.size() - 1];

	// 2 Calculate new tail position
	const Vec2 *segments = aiSnake->getSegments();
	int length = aiSnake->getLength();

	Vec2 currentTail = segments[length - 1];

	// 3 count reachable cells from the new head, while treating the current tail as empty (it will be after one move)
	int reachableCells = countReachable(state, newHead, {currentTail});

	// 4 Safety check: is there enough space for the snake after it grows?
	int requiredSpace = length + 1;

	return reachableCells >= requiredSpace;
}