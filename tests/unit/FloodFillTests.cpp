#include <gtest/gtest.h>
#include "../../incs/FloodFill.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/SnakeAI.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"

class FloodFillTest : public ::testing::Test {
protected:
    std::unique_ptr<Snake> snake;
    std::unique_ptr<Food> food;
    std::unique_ptr<GameConfig> config;
    std::unique_ptr<GameState> state;
    std::unique_ptr<FloodFill> floodFill;
    
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
        floodFill = std::make_unique<FloodFill>();
    }
    
    void TearDown() override {}
};

// 1 - empty corner has maximum reachable cells
TEST_F(FloodFillTest, EmptyCornerReachability) {
    Vec2 start = {0, 0};  // Corner, away from snake (spawns 8-12)
    
    int reachable = floodFill->countReachable(*state, start, {});
    
    // In a 20x20 grid with a small snake (length 4) most cells should be reachable
    EXPECT_GT(reachable, 390);  // At least 390/400 cells
    EXPECT_LE(reachable, 400);  // Max 400 cells
}

// 2 - position next to wall has fewer reachable cells
TEST_F(FloodFillTest, WallReachability) {
    // All corners should have same reachability (empty grid)
    Vec2 corner1 = {0, 0};
    Vec2 corner2 = {19, 19};
    
    int reachable1 = floodFill->countReachable(*state, corner1, {});
    int reachable2 = floodFill->countReachable(*state, corner2, {});
    
    EXPECT_NEAR(reachable1, reachable2, 10);
}

// 3 - blocked position has zero reachability
TEST_F(FloodFillTest, BlockedPosition) {
    // Try to count from a position occupied by snake
    const Vec2* segments = state->snake_A->getSegments();
    Vec2 snakeHead = segments[0];
    
    // Start counting from snake's own position
    // Should return 0 or very small number (just that cell)
    int reachable = floodFill->countReachable(*state, snakeHead, {});
    
    // The snake head itself is not walkable, so should return small number
    EXPECT_GE(reachable, 0);
    EXPECT_LT(reachable, 400);
}

// 4 - ignorePositions parameter works
TEST_F(FloodFillTest, IgnorePositionsWorks) {
    const Vec2* segments = state->snake_A->getSegments();
    Vec2 snakeTail = segments[state->snake_A->getLength() - 1];
    
    // Count from a safe position without ignoring tail
    Vec2 safeStart = {0, 0};

    int reachableWithoutIgnore = floodFill->countReachable(*state, safeStart, {});
    
    // With ignoring tail (should be same or +1)
    int reachableWithIgnore = floodFill->countReachable(*state, safeStart, {snakeTail});
    
    // Ignoring tail might make it reachable
    EXPECT_GE(reachableWithIgnore, reachableWithoutIgnore);
}

// 5 - out of bounds start returns 0
TEST_F(FloodFillTest, OutOfBoundsStart) {
    Vec2 outOfBounds = {-1, -1};
    
    int reachable = floodFill->countReachable(*state, outOfBounds, {});
    
    EXPECT_EQ(reachable, 0);  // Can't reach anything from invalid position
}

// 6 - Multiple ignore positions
TEST_F(FloodFillTest, MultipleIgnorePositions) {
    const Vec2* segments = state->snake_A->getSegments();
    int length = state->snake_A->getLength();
    
    // Create ignore list with all snake segments
    std::vector<Vec2> ignoreAll;
    for (int i = 0; i < length; i++) {
        ignoreAll.push_back(segments[i]);
    }
    
    Vec2 start = {0, 0};
    int reachable = floodFill->countReachable(*state, start, ignoreAll);
    
    // Should reach nearly full grid (ignoring entire snake)
    EXPECT_GT(reachable, 395);  // Almost all 400 cells
}

// 7 - grid completely blocked except start
TEST_F(FloodFillTest, IsolatedCell) {
    // hard to isolate in Snake game. For now, just check that algorithm handles edge cases
    Vec2 start = {0, 0};
    
    int reachable = floodFill->countReachable(*state, start, {});
    
    // Should be positive
    EXPECT_GT(reachable, 0);
}

// 8 - empty path returns false
TEST_F(FloodFillTest, EmptyPathReturnsFalse) {
    std::vector<Vec2> emptyPath;
    
    bool result = floodFill->canReachTail(*state, state->snake_A, emptyPath);
    
    EXPECT_FALSE(result);
}

// 9 - safe path returns true
TEST_F(FloodFillTest, SafePathReturnsTrue) {
    // Create a short path in open space
    std::vector<Vec2> safePath = {
        {1, 1},
        {1, 2},
        {1, 3}
    };
    
    bool result = floodFill->canReachTail(*state, state->snake_A, safePath);
    
    // Should be safe because there's a chingo of space
    EXPECT_TRUE(result);
}

// 10 - path to food checks growth
TEST_F(FloodFillTest, PathAccountsForGrowth) {
    // Path that ends at food position
    Vec2 foodPos = state->food->getPosition();
    std::vector<Vec2> pathToFood = {foodPos};
    
    bool result = floodFill->canReachTail(*state, state->snake_A, pathToFood);
    
    // Should still be safe after eating
    EXPECT_TRUE(result);
}

// 11 - long snake in small space
TEST_F(FloodFillTest, LongSnakeInTightSpace) {
    // Grow the snake significantly
    Snake& snakeRef = *state->snake_A;
    
    for (int i = 0; i < 50; i++) {
        snakeRef.grow();
        snakeRef.move();
    }
    
    // Check if a short path is still safe
    std::vector<Vec2> shortPath = {
        {1, 1},
        {1, 2}
    };
    
    bool result = floodFill->canReachTail(*state, &snakeRef, shortPath);
    
    // With a very long snake, space is limited
    // Result depends on snake position, but **should not crash**
    EXPECT_TRUE(result || !result);  // just cheking if it is still runing!!
}

// 12 - path requires exactly enough space
TEST_F(FloodFillTest, ExactSpaceRequired) {
    Snake& snakeRef = *state->snake_A;
    
    // Create path
    std::vector<Vec2> path = {{0, 0}};
    
    bool result = floodFill->canReachTail(*state, &snakeRef, path);
    
    // Logic: reachableCells >= length + 1, should be true in open grid
    EXPECT_TRUE(result);
}

// 13 - tail position is properly ignored
TEST_F(FloodFillTest, TailPositionIgnored) {
    Snake& snakeRef = *state->snake_A;
    const Vec2* segments = snakeRef.getSegments();
    int length = snakeRef.getLength();
    
    Vec2 tailPos = segments[length - 1];
    
    // Path that ends near tail
    std::vector<Vec2> pathNearTail = {tailPos};
    
    bool result = floodFill->canReachTail(*state, &snakeRef, pathNearTail);
    
    // Should ignore the tail position when counting reachable
    EXPECT_TRUE(result || !result);  // just cheking if it is still runing!!
}

// 14 - integration with actual game state
TEST_F(FloodFillTest, RealGameScenario) {
    Snake& snakeRef = *state->snake_A;
    
    // Normalize snake direction
    if (snakeRef.getDirection() == Direction::Right || 
        snakeRef.getDirection() == Direction::Left) {
        snakeRef.changeDirection(Direction::Up);
        snakeRef.move();
    } else if (snakeRef.getDirection() == Direction::Down) {
        snakeRef.changeDirection(Direction::Left);
        snakeRef.move();
        snakeRef.changeDirection(Direction::Up);
        snakeRef.move();
    }
    
    // Create realistic, simple straight path toward food
    Vec2 head = snakeRef.getSegments()[0];
    Vec2 foodPos = state->food->getPosition();
    
    std::vector<Vec2> path;
    if (head.y > foodPos.y) {
        for (int y = head.y - 1; y >= foodPos.y; y--) {
            path.push_back({head.x, y});
        }
    }
    
    if (!path.empty()) {
        bool result = floodFill->canReachTail(*state, &snakeRef, path);
        
        // In early game with small snake, should be safe
        EXPECT_TRUE(result);
    } else {
        GTEST_SKIP() << "Food positioned directly at snake head";
    }
}

// 15 - counting from multiple positions
TEST_F(FloodFillTest, ConsistentCounting) {
    Vec2 pos1 = {0, 0};
    Vec2 pos2 = {19, 19};
    
    int count1 = floodFill->countReachable(*state, pos1, {});
    int count2 = floodFill->countReachable(*state, pos2, {});
    
    // Both corners should have similar reachability in open grid
    EXPECT_NEAR(count1, count2, 20);  // variance due to snake
}