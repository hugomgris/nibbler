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
#include <cstring>
#include <fstream>
#include <string>

struct AsciiArtFile {
	std::vector<std::string> lines;
	int width;
	int height;
};

class NCursesGraphic : public IGraphic {
private:
	int		width, height;
	WINDOW	*gameWindow;
	bool	isInitialized;
	std::vector<std::vector<char>> groundPattern;  // Stores the ground texture
	
	// Logo Cache
	AsciiArtFile titleSmallA, titleSmallB, titleSmallC, titleSmallD;
	AsciiArtFile titleBigA, titleBigB, titleBigC, titleBigD;
	AsciiArtFile gameoverSmall;
	AsciiArtFile gameoverBig;

	// Drawing functions
	void drawStartScreen(int win_height, int win_width);
	void drawTitle(int win_height, int win_width);
	void drawInstructions(int win_height, int win_width);
	void drawGameOverScreen(const GameState &state, int win_height, int win_width);
	void drawGameOverTitle(int win_height, int win_width);
	void drawGround();
	void drawBorder();
	void drawSnake(const GameState &state);
	void drawFood(const GameState &state);
	void generateGroundPattern();
	
	// Logo drawing helper
	bool loadAsciiArtFile(const std::string& filepath, AsciiArtFile& art);
	
public:
	NCursesGraphic();
	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	~NCursesGraphic();
	
	void init(int w, int h) override;
	void render(const GameState& state, float deltaTime) override;
	void renderMenu(const GameState &state, float deltaTime) override;
	void renderGameOver(const GameState &state, float deltaTime) override;
	Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new NCursesGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
