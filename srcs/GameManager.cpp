#include "../incs/GameManager.hpp"

GameManager::GameManager(GameState *state) : _state(state) {}

void GameManager::update()  {
	_state->snake->move();
	_state->isRunning = checkGameOverCollision();
	checkHeadFoodCollision();
}

void GameManager::calculateDeltaTime(time *lastTime, double* accumulator) {
	auto currentTime = std::chrono::high_resolution_clock::now();
	double deltaTime = std::chrono::duration<double>(currentTime - *lastTime).count();
	
	*lastTime = currentTime;
	*accumulator += deltaTime;
}

bool GameManager::handleGameInput(Input input) {
	switch (input) {
		case Input::Up:
			_state->snake->changeDirection(UP);
			return true;
		case Input::Down:
			_state->snake->changeDirection(DOWN);
			return true;
		case Input::Left:
			_state->snake->changeDirection(LEFT);
			return true;
		case Input::Right:
			_state->snake->changeDirection(RIGHT);
			return true;
		default:
			return false;
	}
}

void GameManager::checkHeadFoodCollision() {
	Vec2	head = _state->snake->getSegments()[0];
	Vec2	foodPos = _state->food->getPosition();

	if (head.x == foodPos.x && head.y == foodPos.y)
	{
		_state->snake->grow();
		_state->food->replace(Utils::getRandomVec2(_state->width, _state->height));
	}
		
}

bool GameManager::checkGameOverCollision()
{
	Vec2	head = _state->snake->getSegments()[0];
	if (head.x < 0 || head.x > _state->width - 1)
		return false;

	if (head.y < 0 || head.y > _state->height - 1)
		return false;

	for (int i = 1; i < _state->snake->getLength(); i++)
	{
		if (_state->snake->getSegments()[i].x == head.x && _state->snake->getSegments()[i].y == head.y)
			return false;
	}

	return true;
}