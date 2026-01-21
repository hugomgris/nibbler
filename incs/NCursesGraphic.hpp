#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include "colors.h"
#include <ncurses.h>
#include <locale.h>
#include <iostream>
#include <vector>
#include <random>

class NCursesGraphic : public IGraphic {
private:
	int		width, height;
	WINDOW	*gameWindow;
	bool	isInitialized;
	std::vector<std::vector<char>> groundPattern;  // Stores the ground texture
	
public:
	NCursesGraphic();
	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	~NCursesGraphic();
	
	void init(int w, int h) override;
	void render(const GameState& state, float deltaTime) override;
	Input pollInput() override;

	// Drawing functions
	void drawGround();
	void drawBorder();
	void drawSnake(const GameState &state);
	void drawFood(const GameState &state);
	void generateGroundPattern();
};

extern "C" IGraphic* createGraphic() {
	return new NCursesGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
