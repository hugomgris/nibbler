#include "../incs/Snake.hpp"
#include <iostream>

Snake::Snake(int width, int height): _length(4), _maxLength((width * height) - 2) {
	_segments = new Vec2[_maxLength];
	
	switch (Utils::getRandomInt(3))
	{
		case 0:
			_direction = Direction::Up;
			break;
		case 1:
			_direction = Direction::Down;
			break;
		case 2:
			_direction = Direction::Left;
			break;
		case 3:
			_direction = Direction::Right;
			break;
	}

	Vec2 headPosition = { Utils::getRandomRangeInt(8, width - 8), Utils::getRandomRangeInt(8, height - 8) };

	switch (_direction) {
		case Direction::Up:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y + 1 };
			_segments[2] = { headPosition.x, headPosition.y + 2 };
			_segments[3] = { headPosition.x, headPosition.y + 3 };
			break;

		case Direction::Down:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y - 1 };
			_segments[2] = { headPosition.x, headPosition.y - 2 };
			_segments[3] = { headPosition.x, headPosition.y - 3 };
			break;

		case Direction::Left:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x + 1, headPosition.y };
			_segments[2] = { headPosition.x + 2, headPosition.y };
			_segments[3] = { headPosition.x + 3, headPosition.y };
			break;

		case Direction::Right:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x - 1, headPosition.y };
			_segments[2] = { headPosition.x - 2, headPosition.y };
			_segments[3] = { headPosition.x - 3, headPosition.y };
			break;
	}
}

Snake::Snake(const Snake &other) : _length(other._length), _maxLength(other._maxLength) {
	_segments = new Vec2[_maxLength];
	_direction = other._direction;
	for (int i = 0; i < _length; ++i) {
		_segments[i] = other._segments[i];
	}
}

Snake &Snake::operator=(const Snake &other) {
	if (this != &other)
	{
		delete[] _segments;
		
		this->_length = other._length;
		this->_maxLength = other._maxLength;
		this->_direction = other._direction;
		this->_segments = new Vec2[_maxLength];
		
		for (int i = 0; i < this->_length; ++i)
			this->_segments[i] = other._segments[i];
	}
	return *this;
}

Snake::~Snake() {
	delete[] _segments;
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
		case Direction::Left:
			head.x--;
			break;

		case Direction::Right:
			head.x++;
			break;

		case Direction::Up:
			head.y--;
			break;

		case Direction::Down:
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

void Snake::changeDirection(Direction dir) { 
	if ((_direction == Direction::Up && dir == Direction::Down)
		|| (_direction == Direction::Down && dir == Direction::Up)
		|| (_direction == Direction::Left && dir == Direction::Right)
		|| (_direction == Direction::Right && dir == Direction::Left))
	{
		return;
	}

	_direction = dir;
};

void Snake::grow() {
	if (_length >= _maxLength) {
		// Snake has filled the entire arena - this is a win condition!
		return;
	}
	_segments[_length] = Vec2{ _segments[_length - 1].x, _segments[_length - 1].y };
	_length++;
}
	
