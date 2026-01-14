#pragma once
#include "DataStructs.hpp"

enum direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Snake {
	private:
		int			_length;
		Vec2		_segments[100];
		direction	_direction;
		int			_hLimit;
		int			_vLimit;

	public:
		Snake() = delete;
		Snake(int width, int height);

		Snake(const Snake &other);
		Snake &operator=(const Snake &other);
		
		~Snake() = default;

		int getLength() const;
		const Vec2 *getSegments() const;

		void move();
		void changeDirection(direction dir);
};