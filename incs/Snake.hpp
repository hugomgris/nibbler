#pragma once
#include "DataStructs.hpp"
#include "Utils.hpp"
#include <array>

enum class Direction {
	Left,
	Right,
	Up,
	Down
};

class Snake {
	private:
		int			_length;
		std::array<Vec2, 100>	_segments;
		Direction	_direction;


	public:
		Snake() = delete;
		Snake(int width, int height);

		Snake(const Snake &other);
		Snake &operator=(const Snake &other);
		
		~Snake() = default;

		int getLength() const;
		const Vec2 *getSegments() const;

		void move();
		void changeDirection(Direction dir);
		void grow();
};