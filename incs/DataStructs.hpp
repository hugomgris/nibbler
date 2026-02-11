#pragma once
#include <cstddef>
#include <memory>
#include <optional>

struct Vec2 {
	int	x;
	int	y;
};

class Snake;
class Food;

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

struct GameConfig {
	GameMode	mode;
};

struct GameState {
	int				width;
	int				height;
	Snake&			snake_A;
	Snake*			snake_B; // Pointer for optional second snake (nullptr in SINGLE mode)
	Food&			food;
	bool			gameOver;
	bool			isRunning;
	bool			isPaused;
	GameStateType	currentState;
	int				score;		// for single player or player A in multiplayer
	int				scoreB;		// for player B/AI in multiplayer
	GameConfig&		config;
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