#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "colors.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

class RaylibGraphic : public IGraphic {
private:
	float cubeSize;
	int gridWidth;
	int gridHeight;
	int screenWidth;
	int screenHeight;
	Camera3D camera;
	
	// Colors
	Color lightSquare = { 180, 180, 200, 255 };
	Color darkSquare = { 120, 120, 140, 255 };
	Color wallColor = { 80, 80, 100, 255 };
	Color wallColorFade = { 80, 80, 100, 150 };
	Color headColor = { 30, 180, 30, 255 };
	Color BodyColor_1 = { 50, 205, 50, 255 };     // Lime green
	Color BodyColor_2 = { 50, 255, 50, 255 };     
	Color foodColor = { 220, 20, 60, 255 };       // Crimson red
	
public:
	RaylibGraphic();
	RaylibGraphic(const RaylibGraphic &other) = delete;
	RaylibGraphic &operator=(const RaylibGraphic &other) = delete;
	~RaylibGraphic();

	void setupCamera(); 
	void drawGroundPlane() ;
	void drawWalls();
	void drawSnake(const Snake* snake);
	void drawFood(const Food* food);

	void init(int width, int height) override;
	void render(const GameState& state) override;
	Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new RaylibGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
