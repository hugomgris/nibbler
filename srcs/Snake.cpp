#include "../incs/Snake.hpp"
#include <iostream>

Snake::Snake(int width, int height): _length(4) {
	switch (Utils::getRandomInt(3))
	{
		case 0:
			_direction = UP;
			break;
		case 1:
			_direction = DOWN;
			break;
		case 2:
			_direction = LEFT;
			break;
		case 3:
			_direction = RIGHT;
			break;
	}

	Vec2 headPosition = { Utils::getRandomRangeInt(8, width - 8), Utils::getRandomRangeInt(8, height - 8) };

	switch (_direction) {
		case UP:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y + 1 };
			_segments[2] = { headPosition.x, headPosition.y + 2 };
			_segments[3] = { headPosition.x, headPosition.y + 3 };
			break;

		case DOWN:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y - 1 };
			_segments[2] = { headPosition.x, headPosition.y - 2 };
			_segments[3] = { headPosition.x, headPosition.y - 3 };
			break;

		case LEFT:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x + 1, headPosition.y };
			_segments[2] = { headPosition.x + 2, headPosition.y };
			_segments[3] = { headPosition.x + 3, headPosition.y };
			break;

		case RIGHT:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x - 1, headPosition.y };
			_segments[2] = { headPosition.x - 2, headPosition.y };
			_segments[3] = { headPosition.x - 3, headPosition.y };
			break;
	}
}

Snake::Snake(const Snake &other)
{
	*this = other;
}

Snake &Snake::operator=(const Snake &other) {
	if (this != &other)
	{
		this->_length = other._length;
		for (int i = 0; i < this->_length; ++i)
			this->_segments[i] = other._segments[i];
	}
	return *this;
}

int Snake::getLength() const { return _length; }

const Vec2 *Snake::getSegments() const { return _segments; }

void Snake::move(){
	auto head = _segments[0];
	Vec2 previousPositions[_length];
	for (int i = 0; i < _length; i++) {
		previousPositions[i] = _segments[i];
	}
	
	switch (_direction)
	{
		case LEFT:
			head.x--;
			break;

		case RIGHT:
			head.x++;
			break;

		case UP:
			head.y--;
			break;

		case DOWN:
			head.y++;
			break;
	}

	_segments[0] = head;

	for (int i = 1; i < _length; ++i)
	{
		Vec2 newPos;
		newPos.x = previousPositions[i - 1].x;
		newPos.y = previousPositions[i - 1].y;
		_segments[i] = newPos;
	}
}

void Snake::changeDirection(direction dir) { 
	if ((_direction == UP && dir == DOWN)
		|| (_direction == DOWN && dir == UP)
		|| (_direction == LEFT && dir == RIGHT)
		|| (_direction == RIGHT && dir == LEFT))
	{
		return;
	}

	_direction = dir;
};

void Snake::grow() {
	// TODO: take into consideration the limit, or make the limit itself the endgame condition
	_segments[_length] = Vec2{ _segments[_length - 1].x, _segments[_length - 1].y };
	_length++;
}
	
