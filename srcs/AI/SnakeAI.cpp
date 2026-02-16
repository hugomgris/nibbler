#include "../../incs/SnakeAI.hpp"
#include "../../incs/Food.hpp"
#include <cstdlib>
#include <ctime>

SnakeAI::SnakeAI(AIConfig config) : config(config) {
	// easy mode's random component needs to be seeded
	std::srand(std::time(nullptr));
}

Input positionToInput(Vec2 from, Vec2 to) {
	int dx = to.x - from.x;
	int dy = to.y - from.y;
	
	// Determine direction based on delta
	if (dx > 0) return Input::Right_B;  // Moving right
	if (dx < 0) return Input::Left_B;   // Moving left
	if (dy > 0) return Input::Down_B;   // Moving down
	if (dy < 0) return Input::Up_B;     // Moving up
	
	return Input::None;  // No movement
}

Vec2 getNextPosition(Vec2 head, Input direction) {
	switch (direction) {
		case Input::Up_B:    return {head.x, head.y - 1};
		case Input::Down_B:  return {head.x, head.y + 1};
		case Input::Left_B:  return {head.x - 1, head.y};
		case Input::Right_B: return {head.x + 1, head.y};
		default:             return head;
	}
}

Input SnakeAI::goToFood(const GameState& state) {
	if (!state.snake_B) return Input::None;
	
	Vec2 head = state.snake_B->getSegments()[0];
	Vec2 foodPos = state.food->getPosition();
	
	// find food path
	std::vector<Vec2> path = pathFinder.findPath(state, head, foodPos, config.maxSearchDepth);
	
	if (!path.empty()) {
		// tail reachable check
		if (config.useSafetyCheck) {
			if (!floodFill.canReachTail(state, state.snake_B, path)) {
				// if the path is unsafe, go into survival mode (square movement, basically)
				return survivalMode(state);
			}
		}

		return positionToInput(head, path[0]);
	}
	
	// no food path triggers survival mode too
	return survivalMode(state);
}

Input SnakeAI::survivalMode(const GameState& state) {
	if (!state.snake_B) return Input::None;
	
	Vec2 head = state.snake_B->getSegments()[0];
	const Vec2* segments = state.snake_B->getSegments();
	int length = state.snake_B->getLength();
	Vec2 tail = segments[length - 1];
	
	// try to reach tail because it's always moving, i.e. it is always a safe spot
	std::vector<Vec2> pathToTail = pathFinder.findPath(state, head, tail, config.maxSearchDepth);
	
	if (!pathToTail.empty()) {
		return positionToInput(head, pathToTail[0]);
	}
	
	// If tail is not reachable, just go towards open space
	return maximizeSpace(state);
}

Input SnakeAI::maximizeSpace(const GameState& state) {
	if (!state.snake_B) return Input::None;
	
	Vec2 head = state.snake_B->getSegments()[0];
	
	// pick the direction (among the 4 available) with most open space
	int bestSpace = -1;
	Input bestMove = Input::None;
	
	Input directions[] = {Input::Up_B, Input::Down_B, Input::Left_B, Input::Right_B};
	
	for (Input dir : directions) {
		Vec2 nextPos = getNextPosition(head, dir);
		
		// Check move safety
		if (isSafeMove(state, nextPos)) {
			// If it is sae, count the reachable spots
			int space = floodFill.countReachable(state, nextPos, {});
			
			if (space > bestSpace) {
				bestSpace = space;
				bestMove = dir;
			}
		}
	}
	
	return bestMove;
}

bool SnakeAI::isSafeMove(const GameState& state, Vec2 nextPos) {
	// Bounds check
	if (nextPos.x < 0 || nextPos.y < 0 || 
		nextPos.x >= state.width || nextPos.y >= state.height) {
		return false;
	}
	
	// self collisions check
	if (state.snake_B) {
		const Vec2* segments = state.snake_B->getSegments();
		int length = state.snake_B->getLength();
		
		for (int i = 0; i < length - 1; i++) {      // -1 because tail will move
			if (segments[i].x == nextPos.x && segments[i].y == nextPos.y) {
				return false;
			}
		}
	}
	
	// opponent collisions check
	const Vec2* opponentSegs = state.snake_A->getSegments();
	int opponentLen = state.snake_A->getLength();
	
	for (int i = 0; i < opponentLen; i++) {
		if (opponentSegs[i].x == nextPos.x && opponentSegs[i].y == nextPos.y) {
			return false;
		}
	}
	
	return true;
}

int SnakeAI::evaluateMoveSpace(const GameState& state, Vec2 nextPos) {
	return floodFill.countReachable(state, nextPos, {});
}

Input SnakeAI::decideNextMove(const GameState& state) {
	if (!state.snake_B) return Input::None;
	
	// EASY MODE → Random moves sometimes
	if (config.level == AIConfig::EASY && config.randomMoveChance > 0.0f) {
		float roll = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		if (roll < config.randomMoveChance) {
			// random move must still be valid, unless we want artificial AI deaths (WE DONT)
			return maximizeSpace(state);
		}
	}
	
	// decision hierarchy:
	// 1 - Try to go to food
	Input foodMove = goToFood(state);
	if (foodMove != Input::None) {
		return foodMove;
	}
	
	// 2 - If can't get food safely, go into survival mode
	Input survivalMove = this->survivalMode(state);
	if (survivalMove != Input::None) {
		return survivalMove;
	}
	
	// 3 - just D O N T   D I E
	Input spaceMove = maximizeSpace(state);
	if (spaceMove != Input::None) {
		return spaceMove;
	}
	
	// 4 - AI is turbo cooked, but something  needs to be returned
	return Input::Left_B;
}