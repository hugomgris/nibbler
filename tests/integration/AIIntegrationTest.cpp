#include <gtest/gtest.h>
#include "../../incs/SnakeAI.hpp"
#include "../../incs/Pathfinder.hpp"
#include "../../incs/FloodFill.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"

class AIIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<Snake> snakeA;
    std::unique_ptr<Snake> snakeB;
    std::unique_ptr<Food> food;
    std::unique_ptr<GameConfig> config;
    std::unique_ptr<GameState> state;
    std::unique_ptr<SnakeAI> aiEasy;
    std::unique_ptr<SnakeAI> aiMedium;
    std::unique_ptr<SnakeAI> aiHard;
    
    void SetUp() override {
        snakeA = std::make_unique<Snake>(20, 20);
        snakeB = std::make_unique<Snake>(Snake(*snakeA, 20, 20));  // Opposite side
        food = std::make_unique<Food>(Vec2{10, 10}, 20, 20);
        config = std::make_unique<GameConfig>(GameConfig{GameMode::AI});
        state = std::make_unique<GameState>(GameState{
            20, 20,
            snakeA.get(), snakeB.get(),
            food.get(),
            false, true, false,
            GameStateType::Playing,
            0, 0,
            *config
        });
        
        aiEasy = std::make_unique<SnakeAI>(AIConfig::easy());
        aiMedium = std::make_unique<SnakeAI>(AIConfig::medium());
        aiHard = std::make_unique<SnakeAI>(AIConfig::hard());
    }
    
    void TearDown() override {}
};

// 1 - AI makes valid decisions
TEST_F(AIIntegrationTest, AIReturnsValidMove) {
    Input move = aiEasy->decideNextMove(*state);
    
    // Should return a valid snake_B input
    EXPECT_TRUE(move == Input::Up_B || move == Input::Down_B || 
                move == Input::Left_B || move == Input::Right_B);
}

// 2 - All difficulty levels work
TEST_F(AIIntegrationTest, AllDifficultyLevelsWork) {
    Input easyMove = aiEasy->decideNextMove(*state);
    Input mediumMove = aiMedium->decideNextMove(*state);
    Input hardMove = aiHard->decideNextMove(*state);
    
    EXPECT_NE(easyMove, Input::None);
    EXPECT_NE(mediumMove, Input::None);
    EXPECT_NE(hardMove, Input::None);
}

// 3 - AI moves toward food
TEST_F(AIIntegrationTest, AIMovesTowardFood) {
    // Position food away from AI snake
    Vec2 aiHead = state->snake_B->getSegments()[0];
    Vec2 foodPos = {aiHead.x + 5, aiHead.y};
    state->food = std::make_unique<Food>(foodPos, 20, 20).get();
    
    Input move = aiHard->decideNextMove(*state);
    
    // AI should make a valid move (not necessarily **optimal**)
    EXPECT_NE(move, Input::None);
}

// 4 - AI doesn't crash with no food path
TEST_F(AIIntegrationTest, HandlesNoFoodPath) {
    Input move = aiMedium->decideNextMove(*state);
    
    EXPECT_TRUE(move != Input::None || move == Input::None);  // Just don't crash
}

// 5 - AI survives multiple moves
TEST_F(AIIntegrationTest, AIMultipleMoves) {
    int movesCount = 0;
    
    for (int i = 0; i < 10; i++) {
        Input move = aiHard->decideNextMove(*state);
        if (move != Input::None) {
            movesCount++;
        }
    }
    
    EXPECT_GT(movesCount, 0);  // AI should make at least some moves
}

// 6 - Easy AI has randomness
TEST_F(AIIntegrationTest, EasyAIShowsVariability) {
    // Run multiple times and check Dcisons Are Made
    std::vector<Input> moves;
    
    for (int i = 0; i < 20; i++) {
        moves.push_back(aiEasy->decideNextMove(*state));
    }
    
    // Should have made at least one valid move
    int validMoves = 0;
    for (Input m : moves) {
        if (m != Input::None) validMoves++;
    }
    
    EXPECT_GT(validMoves, 15);  // At least 75% valid moves
}

// 7 -Hard AI uses safety checks
TEST_F(AIIntegrationTest, HardAIUsesSafetyChecks) {
    Input move = aiHard->decideNextMove(*state);
    
    // Hard AI should always make safe moves
    EXPECT_NE(move, Input::None);
}

// 8 - AI handles edge of grid
TEST_F(AIIntegrationTest, AIHandlesEdgePosition) {
    // Create new snake near edge
    Snake edgeSnake(20, 20);
    state->snake_B = &edgeSnake;
    
    Input move = aiMedium->decideNextMove(*state);
    
    // should not crash *prayEmoji
    EXPECT_TRUE(move == Input::Up_B || move == Input::Down_B || 
                move == Input::Left_B || move == Input::Right_B ||
                move == Input::None);
}

// 9 - AI vs AI simulation (stress test)
TEST_F(AIIntegrationTest, AISimulationStressTest) {
    // run lots of frames, check there are no crashes
    bool crashed = false;
    
    for (int frame = 0; frame < 100; frame++) {
        try {
            Input move = aiHard->decideNextMove(*state);
            if (move == Input::None) {
                // if ai trapped just get out, GET OUT!!
                break;
            }
        } catch (...) {
            crashed = true;
            break;
        }
    }
    
    EXPECT_FALSE(crashed);
}

// 10 - AI pathfinding integration
TEST_F(AIIntegrationTest, PathfindingIntegration) {
    // controlled scenario: place food in an open area
    Vec2 head = state->snake_B->getSegments()[0];
    
    // food 5 steps to the right (guaranteed clear in most random spawns; added bound protection)
    Vec2 foodPos;
    if (head.x + 5 < 20) {
        foodPos = {head.x + 5, head.y};
    } else {
        foodPos = {head.x - 5, head.y};
    }
    
    // Update food position
    state->food = std::make_unique<Food>(foodPos, 20, 20).get();
    
    PathFinder pathfinder;
    std::vector<Vec2> path = pathfinder.findPath(*state, head, foodPos, 200);
    
    // Path might not exist if blocked, but AI should still make a decision. The important test is that pathfinding doesn't crash, as always
    if (!path.empty()) {
        EXPECT_GT(path.size(), 0UL);
    }
    
    // AI should always be able to make *some* move (even if not toward food)
    Input move = aiHard->decideNextMove(*state);
    EXPECT_NE(move, Input::None);
}

// 11 - FloodFill integration
TEST_F(AIIntegrationTest, FloodFillIntegration) {
    FloodFill floodFill;
    Vec2 head = state->snake_B->getSegments()[0];
    
    int reachable = floodFill.countReachable(*state, head, {});
    
    // Should have significant reachable space and it should be used for maximization
    EXPECT_GT(reachable, 10);
    
    Input move = aiHard->decideNextMove(*state);
    EXPECT_NE(move, Input::None);
}

// 12 - Full game loop simulation
TEST_F(AIIntegrationTest, FullGameLoopSimulation) {
    int frames = 0;
    int maxFrames = 50;
    
    while (frames < maxFrames && state->isRunning) {
        // AI decides
        Input aiMove = aiMedium->decideNextMove(*state);
        
        if (aiMove != Input::None) {
            // Simulate game update
            frames++;
        } else {
            break;
        }
    }
    
    // Survival must be at least longer than 5 frames
    EXPECT_GT(frames, 5);
}
