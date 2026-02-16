#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"
#include "../../incs/GameController.hpp"

class SnakeCollisionTest : public ::testing::Test {
	protected:
		std::unique_ptr<Snake> snake;
		std::unique_ptr<Food> food;
		std::unique_ptr<GameConfig> config;
		std::unique_ptr<GameState> state;
		std::unique_ptr<GameController> manager;
		
		void SetUp() override {
			snake = std::make_unique<Snake>(20, 20);
			food = std::make_unique<Food>(Vec2{10, 10}, 20, 20);
			config = std::make_unique<GameConfig>(GameConfig{GameMode::SINGLE});
			state = std::make_unique<GameState>(GameState{
				20, 20,
				snake.get(), nullptr,
				food.get(),
				false, true, false,
				GameStateType::Playing,
				0, 0,
				*config
			});
			manager = std::make_unique<GameController>(state.get());
		}
		
		void TearDown() override {}
};

TEST_F(SnakeCollisionTest, SelfCollision) {
	Snake &snakeRef = *state->snake_A;
	
	if (snakeRef.getDirection() != Direction::Right)
		snakeRef.changeDirection(Direction::Left);
	else {
		snakeRef.changeDirection(Direction::Up);
		manager->update();
		snakeRef.changeDirection(Direction::Left);
	}

	manager->update();
	snakeRef.grow();
	manager->update();
	snakeRef.grow();
	manager->update();
	snakeRef.grow();
	manager->update();
	snakeRef.grow();
	manager->update();
	snakeRef.grow();

	// Check if already colliding
	manager->update();

	if (snakeRef.getDirection() != Direction::Right)
		snakeRef.changeDirection(Direction::Left);
	manager->update();
	manager->update();
	snakeRef.changeDirection(Direction::Down);
	manager->update();
	manager->update();
	snakeRef.changeDirection(Direction::Right);
	manager->update();
	manager->update();
	snakeRef.changeDirection(Direction::Up);
	manager->update();
	manager->update();

	EXPECT_FALSE(state->isRunning);
}

TEST_F(SnakeCollisionTest, WallCollision) {
	Snake &snakeRef = *state->snake_A;

	// Normalize direction to go upwards
	if (snakeRef.getDirection() == Direction::Right || snakeRef.getDirection() == Direction::Left) {
		snakeRef.changeDirection(Direction::Up);
		manager->update();
	} else if (snakeRef.getDirection() == Direction::Down) {
		snakeRef.changeDirection(Direction::Left);
		manager->update();
		snakeRef.changeDirection(Direction::Up);
		manager->update();
	}

	// Move until boundary -> y = 0
	while (snakeRef.getSegments()[0].y > 0) {
		manager->update();
	}
	
	// C O L L I D E
	manager->update();

	EXPECT_FALSE(state->isRunning);
	EXPECT_EQ(snakeRef.getSegments()[0].y, -1);
}

