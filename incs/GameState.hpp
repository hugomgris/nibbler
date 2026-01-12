#pragma once
#include <cstddef>

/*
This is the Day One prototype, view-only, i.e. no logic nor ownership transfer.
It will evolve, but for now it respects these rules:
	-Only POD (Plain Old Data) types
	-No STL
	-Main owns memory
	-Libraries only read
*/

struct Vec2 {
	int	x;
	int	y;
};

struct SnakeView {
	const Vec2	*segments;
	std::size_t	length;
};

struct FoodView {
	Vec2 position;
};

struct GameState {
	int			width;
	int			height;
	SnakeView	snake;
	FoodView	food;
	bool		gameOver;
};