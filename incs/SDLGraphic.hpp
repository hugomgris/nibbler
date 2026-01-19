#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "colors.h"
#include <SDL2/SDL.h>
#include <iostream>

class SDLGraphic : public IGraphic {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	int cellSize;
	
public:
	SDLGraphic();
	SDLGraphic(const SDLGraphic&) = delete;
	SDLGraphic &operator=(const SDLGraphic&) = delete;
	~SDLGraphic();
	
	void init(int width, int height) override;
	void render(const GameState& state) override;
	Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new SDLGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}