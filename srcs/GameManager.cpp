#include "../incs/GameManager.hpp"

GameManager::GameManager(Snake *snake) : _snake(snake) {}

void GameManager::update()  {
	_snake->move();
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
			_snake->changeDirection(UP);
			return true;
		case Input::Down:
			_snake->changeDirection(DOWN);
			return true;
		case Input::Left:
			_snake->changeDirection(LEFT);
			return true;
		case Input::Right:
			_snake->changeDirection(RIGHT);
			return true;
		default:
			return false;
	}
}
