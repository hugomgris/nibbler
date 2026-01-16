#include "../../incs/IGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include <raylib.h>
#include <iostream>

class RaylibGraphic : public IGraphic {
private:
	int cellSize;
	int screenWidth;
	int screenHeight;
	
public:
	RaylibGraphic() : cellSize(20), screenWidth(0), screenHeight(0) {}
	
	void init(int width, int height) override {
		screenWidth = width * cellSize;
		screenHeight = height * cellSize;
		
		InitWindow(screenWidth, screenHeight, "Nibbler - Raylib");
		SetTargetFPS(0);
		
		std::cout << BYEL << "[Raylib] Initialized: " << width << "x" << height << RESET << std::endl;
	}
	
	void render(const GameState& state) override {
		BeginDrawing();
		ClearBackground(BLACK);
		
		// Draw snake (yellow)
		for (int i = 0; i < state.snake->getLength(); ++i) {
			DrawRectangle(
				state.snake->getSegments()[i].x * cellSize,
				state.snake->getSegments()[i].y * cellSize,
				cellSize,
				cellSize,
				YELLOW
			);
		}
		
		// Draw food (blue)
		DrawRectangle(
			state.food->getPosition().x * cellSize,
			state.food->getPosition().y * cellSize,
			cellSize,
			cellSize,
			BLUE
		);
		
		EndDrawing();
	}
	
	Input pollInput() override {
		if (IsKeyPressed(KEY_UP))		return Input::Up;
		if (IsKeyPressed(KEY_DOWN))		return Input::Down;
		if (IsKeyPressed(KEY_LEFT))		return Input::Left;
		if (IsKeyPressed(KEY_RIGHT))	return Input::Right;
		if (IsKeyPressed(KEY_Q))		return Input::Quit;
		if (IsKeyPressed(KEY_ESCAPE))	return Input::Quit;
		if (IsKeyPressed(KEY_ONE))		return Input::SwitchLib1;
		if (IsKeyPressed(KEY_TWO))		return Input::SwitchLib2;
		if (IsKeyPressed(KEY_THREE))	return Input::SwitchLib3;
		
		if (WindowShouldClose())		return Input::Quit;
		
		return Input::None;
	}
	
	void cleanup() override {
		// Raylib cleanup happens in destructor
	}
	
	~RaylibGraphic() {
		CloseWindow();
		std::cout << BYEL << "[Raylib] Destroyed" << RESET << std::endl;
	}
};

extern "C" IGraphic* createGraphic() {
	return new RaylibGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
