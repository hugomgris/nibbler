#include "../incs/Food.hpp"
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

bool Food::replace(Vec2 newPos)
{
	if ((newPos.x < 0 || newPos.x > _hLimit - 1) ||
		(newPos.y < 0 || newPos.y > _vLimit - 1))
	{
		std::cerr << "Invalid coordinates for food reposition - (" << newPos.x << "," << newPos.y << ")" << std::endl;
		return false;
	}

	_position = newPos;
	_foodChar = Utils::getFoodChar(Utils::getRandomInt(5));
	return true;
}

Vec2 Food::getPosition() const { return _position; }

const char *Food::getFoodChar() const { return _foodChar; };
