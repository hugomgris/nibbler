#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"
#include "../../incs/GameController.hpp"

class FoodInteractionTest : public ::testing::Test {
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

TEST_F(FoodInteractionTest, FoodPositioning) {
	Snake &snakeRef = *state->snake_A;
	Food &foodRef = *state->food;

	// moving the snake a bit

	for (int i = 3; i > 0; i--)
		snakeRef.move();

	// the key thing here is the repositioning of the food, which triggers when eating happens
	// after a repositioning, the food should always be in bounds and never be in a snake-occupied spot
	for (int i = 5; i > 0; i--) {
		foodRef.replaceInFreeSpace(state.get());

		int x = foodRef.getPosition().x;
		int y = foodRef.getPosition().y;

		EXPECT_GE(x, 0);
		EXPECT_LT(x, 20);
		EXPECT_GE(y, 0);
		EXPECT_LT(y, 20);

		const Vec2 *segments = snakeRef.getSegments();
		int snakeLength = snakeRef.getLength();

		while (--snakeLength > 0)
		{
			EXPECT_FALSE(segments[snakeLength].x == x && segments[snakeLength].y == y);
		}
	}
}

TEST_F(FoodInteractionTest, EatingTrigger) {
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
	
	//Force a food position ABOVE the snake head and to its RIGHT
	while (state->food->getPosition().x <= snakeRef.getSegments()[0].x || state->food->getPosition().y >= snakeRef.getSegments()[0].y)
		state->food->replaceInFreeSpace(state.get());

	Vec2 foodPosition = state->food->getPosition();

	EXPECT_TRUE(state->food->getPosition().x > snakeRef.getSegments()[0].x && state->food->getPosition().y < snakeRef.getSegments()[0].y);

	// Move the head of the snake to collide with the food
	int i = state->snake_A->getSegments()[0].y - state->food->getPosition().y;

	while (i > 0) {
		manager->update();
		i--;
	}

	snakeRef.changeDirection(Direction::Right);
	i = state->food->getPosition().x - state->snake_A->getSegments()[0].x;

	// midway check -> food should still be in the same position
	EXPECT_TRUE(foodPosition.x == state->food->getPosition().x && foodPosition.y == state->food->getPosition().y);

	while (i > 0) {
		manager->update();
		i--;
	}
	
	Vec2 newFoodPosition = state->food->getPosition();

	if (foodPosition.x == newFoodPosition.x)
		EXPECT_TRUE(foodPosition.y != newFoodPosition.y);
	else if (foodPosition.y == newFoodPosition.y)
		EXPECT_TRUE(foodPosition.x != newFoodPosition.x);
	else {
		EXPECT_TRUE(foodPosition.x != newFoodPosition.x);
		EXPECT_TRUE(foodPosition.y != newFoodPosition.y);
	}
	
}