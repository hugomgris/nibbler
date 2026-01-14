#include "../incs/GameManager.hpp"

GameManager::GameManager(Snake *snake) : _snake(snake) {}

void GameManager::moveSnake()  {
	_snake->move();
}


