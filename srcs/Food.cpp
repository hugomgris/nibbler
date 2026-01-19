#include "../incs/Food.hpp"
#include "../incs/Snake.hpp"
#include <iostream>

Food::Food(Vec2 position, int width, int height) : _position(position), _hLimit(width), _vLimit(height) {
	_foodChar = Utils::getFoodChar(Utils::getRandomInt(5));
}

Food::Food(const Food &other)
{
	*this = other;
}

Food &Food::operator=(const Food & other)
{
	if (this != &other)
	{
		this->_position = other._position;
	}

	return *this;
}

bool Food::replaceInFreeSpace(GameState *gameState)
{
	std::vector<Vec2> snakeSegments;
	for (int i = 0; i < gameState->snake->getLength(); i++) {
		snakeSegments.push_back(gameState->snake->getSegments()[i]);
	}

	std::vector<Vec2> availableCells;
	availableCells.reserve(_hLimit * _vLimit - snakeSegments.size());

	for (int y = 0; y < _vLimit; y++) {
		for (int x = 0; x < _hLimit; x++)
		{
			Vec2 candidate = {x, y};

			bool occupied = false;
			for (const auto &segment : snakeSegments) {
				if (segment.x == candidate.x && segment.y == candidate.y)
				{
					occupied = true;
					break;
				}
			}

			if (!occupied) {
				availableCells.push_back(candidate);
			}
		}
	}

	if (availableCells.empty())
	{
		std::cout << "No available cells! aka you Won, bb" << std::endl;
		return false;
	}

	int randomIndex = Utils::getRandomInt(availableCells.size());
	_position = availableCells[randomIndex];
	_foodChar = Utils::getFoodChar(Utils::getRandomInt(5));

	return true;
}

Vec2 Food::getPosition() const { return _position; }

const char *Food::getFoodChar() const { return _foodChar; };
