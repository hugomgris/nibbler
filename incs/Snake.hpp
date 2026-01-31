#pragma once
#include "DataStructs.hpp"
#include "Utils.hpp"

enum class Direction {
	Left,
	Right,
	Up,
	Down
};

class Snake {
	private:
		int			_length;
		int			_maxLength;
		Vec2		*_segments;
		Direction	_direction;


	public:
		Snake() = delete;
		Snake(int width, int height);

		Snake(const Snake &other);
		Snake &operator=(const Snake &other);
		
		~Snake();

		int getLength() const;
		const Vec2 *getSegments() const;

		void move();
		void changeDirection(Direction dir);
		void grow();
};