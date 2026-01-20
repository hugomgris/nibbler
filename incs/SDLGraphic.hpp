#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "colors.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>

struct BorderLine {
	float progress;        // 0 = at arena, 1 = at window edge
	float age;
	
	BorderLine() : progress(0.0f), age(0.0f) {}
};

class SDLGraphic : public IGraphic {
private:
	SDL_Window		*window;
	SDL_Renderer	*renderer;
	int				gridWidth;
	int				gridHeight;
	int				cellSize;
	int				borderOffset;

	// Tunnel effect animation
	std::vector<BorderLine> borderLines;
	std::chrono::high_resolution_clock::time_point lastSpawnTime;
	float spawnInterval;
	float animationSpeed;
	bool enableTunnelEffect;

	// Colors
	SDL_Color customWhite = { 255, 248, 227, 255};	// Off-white
	SDL_Color customBlack = { 23, 23, 23, 255};		// Charcoal black

	SDL_Color lightRed = { 254, 74, 81, 255 };
	//SDL_Color darkRed = { 180, 52, 58, 255 };

	SDL_Color lightBlue = { 70, 130, 180, 255 };
	//SDL_Color darkBlue = { 18, 45, 68, 255 };
	
	// Helper function to set render color from SDL_Color
	void setRenderColor(SDL_Color color, bool customAlpha = false, Uint8 alphaValue = 255);
	
	// Tunnel effect helper functions
	void updateTunnelEffect(float deltaTime);
	void renderTunnelEffect();
	float easeInQuad(float t);
	
public:
	SDLGraphic();
	SDLGraphic(const SDLGraphic&) = delete;
	SDLGraphic &operator=(const SDLGraphic&) = delete;
	~SDLGraphic();
	
	void init(int width, int height) override;
	void render(const GameState& state) override;
	Input pollInput() override;

	// Drawing functions
	void drawBorder(int thickness);
};

extern "C" IGraphic* createGraphic() {
	return new SDLGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}