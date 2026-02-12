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
		bool		_isDead = false;


	public:
		Snake() = delete;
		Snake(int width, int height);
		Snake(const Snake &otherSnake, int width, int height);	// multiplayer, second snake constructor (opposite)
		Snake(Snake &&other) noexcept;
		Snake &operator=(Snake &&other) noexcept;
		Snake(const Snake &other);
		Snake &operator=(const Snake &other);
		
		~Snake();

		void setAsDead(bool dead);
		bool isDead() const;

		int getLength() const;
		const Vec2 *getSegments() const;
		Direction getDirection() const;  //for testing

	void move();
	void changeDirection(Direction dir);
	void grow();
	void reset(int width, int height);  // Reinitialize snake at new position
	void resetAsMirrored(const Snake& otherSnake, int width, int height);  // Reset as mirrored opponent

private:
	void initializeAtRandomPosition(int width, int height);  // Common spawn logic
	void initializeAsMirrored(const Snake& otherSnake, int width, int height);  // Initialize as mirrored opponent
};