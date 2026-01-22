#pragma once
#include <cstddef>

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

struct GameState {
	int				width;
	int				height;
	Snake			*snake;
	Food			*food;
	const char		*foodChar;
	bool			gameOver;
	bool			isRunning;
	bool			isPaused;
	GameStateType	currentState;
	int				score;
};
