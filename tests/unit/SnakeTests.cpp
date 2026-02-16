#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/SnakeAI.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"

class SnakeTest : public ::testing::Test {
	protected:
		std::unique_ptr<Snake> snake;
    
		void SetUp() override {
			snake = std::make_unique<Snake>(20, 20);
		}
};

TEST_F(SnakeTest, SnakeInitialization) {
	EXPECT_EQ(snake->getLength(), 4);
}

TEST_F(SnakeTest, SnakeMovement) {
	Direction dir = snake->getDirection();
	Vec2 initialPos = snake->getSegments()[0];

	snake->move();
	Vec2 newPos = snake->getSegments()[0];

	if (dir == Direction::Left || dir == Direction::Right)
		EXPECT_NE(initialPos.x, newPos.x);
	else
		EXPECT_NE(initialPos.y, newPos.y);
}

TEST_F(SnakeTest, SnakeGrowth) {
	int initialLength = snake->getLength();

	snake->grow();
	int newLength = snake->getLength();

	EXPECT_FALSE(initialLength == newLength);
	EXPECT_EQ(initialLength, 4);
	EXPECT_EQ(newLength, 5);

	for (int i = 0; i < 10; i++)
		snake->grow();

	EXPECT_EQ(snake->getLength(), 15);
}

TEST_F(SnakeTest, DirectionControl) {
	if (snake->getDirection() == Direction::Right || snake->getDirection() == Direction::Left) {
		snake->changeDirection(Direction::Up);
	} else if (snake->getDirection() == Direction::Down) {
		snake->changeDirection(Direction::Left);
		snake->changeDirection(Direction::Up);
	}

	// forcing an upwards direction to control the test state
	EXPECT_EQ(snake->getDirection(), Direction::Up);

	// the exact opposite direction shouldn't be switchable, in this case the snake shouldn't be able to go from up to down
	snake->changeDirection(Direction::Down);
	EXPECT_EQ(snake->getDirection(), Direction::Up);

	// regular changing tests
	snake->changeDirection(Direction::Right);
	EXPECT_EQ(snake->getDirection(), Direction::Right);

	snake->changeDirection(Direction::Down);
	snake->changeDirection(Direction::Left);
	EXPECT_EQ(snake->getDirection(), Direction::Left);
}

TEST_F(SnakeTest, GrowthPattern) {

	if (snake->getDirection() == Direction::Right || snake->getDirection() == Direction::Left) {
		snake->changeDirection(Direction::Up);
	} else if (snake->getDirection() == Direction::Down) {
		snake->changeDirection(Direction::Left);
		snake->changeDirection(Direction::Up);
	}

	// forcing an upwards direction to control the test state
	EXPECT_EQ(snake->getDirection(), Direction::Up);

	int growths = snake->getSegments()[0].y - 1;
	int accumulated = 0;

	while (growths > 0 && snake->getSegments()[0].y > 0)
	{
		snake->grow();
		snake->move();
		growths--;
		accumulated++;
	}

	// at this point, the snake should have grown a couple of times and be at y = 0;
	EXPECT_EQ(snake->getLength(), 4 + accumulated);

	// moving down and stretching the snake all the way
	snake->changeDirection(Direction::Right);
	snake->move();
	snake->changeDirection(Direction::Down);
	snake->move();

	while ((snake->getSegments()[0].x != snake->getSegments()[snake->getLength() - 1].x) && snake->getSegments()[0].y < 19)
		snake->move();

	EXPECT_EQ(snake->getSegments()[0].x,  snake->getSegments()[snake->getLength() - 1].x);
}