#pragma once
#include <cstddef>
#include <memory>
#include <optional>

struct Vec2 {
	int	x;
	int	y;
};

// Forward declarations
class Snake;
class Food;
class SnakeAI;

enum class GameStateType {
    Menu,
    Playing,
    Paused,
    GameOver
};

enum class GameMode {
	SINGLE,
	MULTI,
	AI
};

enum class RenderMode {
	ASCII,
	MODE2D,
	MODE3D
};

struct GameConfig {
	GameMode	mode;
};

struct GameTiming {
	float accumulator = 0.0f;
	float fixedTimestep = 1.0f / 10.0f;
	float lastFrameTime = 0.0f;
};

struct GameState {
	int							width;
	int							height;
	Snake*						snake_A;
	Snake*						snake_B;
	Food*						food;
	bool						gameOver;
	bool						isRunning;
	bool						isPaused;
	GameStateType				currentState;
	int							score;
	int							scoreB;
	GameConfig					config;
	GameTiming					timing;
	RenderMode					renderMode;
	std::unique_ptr<SnakeAI>	aiController;
};

struct AIConfig {
	// this is were difficulty settings are, well, set
	enum Difficulty { EASY, MEDIUM, HARD };
	
	Difficulty level;
	
	// Timing
	int thinkDelay;			// Ticks between decisions (0 = every tick)
	
	// Pathfinding
	int maxSearchDepth;		// Node expansion limit
	bool useSafetyCheck;	// Tail reachability
	bool predictOpponent;	// Consider opponent movement
	
	// Behavior
	float randomMoveChance;	// 0.0 - 1.0 (for easy mode)
	float aggressiveness;	// 0.0 = cautious, 1.0 = greedy
	
	static AIConfig easy();
	static AIConfig medium();
	static AIConfig hard();
};