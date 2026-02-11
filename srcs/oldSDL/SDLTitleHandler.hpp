#pragma once

#include <SDL.h>
#include <vector>

class SDLTitleHandler {
private:
	SDL_Renderer* renderer;
	
	// Helper to draw multiple rectangles at once
	void drawRects(const std::vector<SDL_Rect>& rects, SDL_Color color);

public:
	SDLTitleHandler();
	SDLTitleHandler(SDL_Renderer* renderer);
	SDLTitleHandler(const SDLTitleHandler &other) = delete;
	SDLTitleHandler &operator=(const SDLTitleHandler &other) = delete;

	~SDLTitleHandler() = default;

	void renderTitle(int centerX, int centerY, int square, int sep, SDL_Color white, SDL_Color blue, SDL_Color red);
	void renderGameOver(int centerX, int centerY, int square, int sep, SDL_Color white, SDL_Color gray);

	void drawGameOver();
};
