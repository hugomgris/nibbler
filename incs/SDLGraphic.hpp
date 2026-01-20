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

struct DustParticle {
	float x, y;				// Center position
	float rotation;          
	float rotationSpeed;     
	float initialSize;
	float currentSize;
	float lifetime;
	float age;
	
	DustParticle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime) 
		: x(px), y(py), age(0.0f) {
		initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
		currentSize = initialSize;
		lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
		rotation = static_cast<float>(rand() % 360);
		rotationSpeed = -30.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 60.0f;  // -30 to +30 deg/s
	}
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

	// Dust particle system
	std::vector<DustParticle> dustParticles;
	int maxDustDensity;
	float dustSpawnInterval;
	float dustSpawnTimer;
	float dustMinSize;
	float dustMaxSize;
	float dustMinLifetime;
	float dustMaxLifetime;

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

	// Dust particle system helper functions
	void updateDustParticles(float deltaTime);
	void renderDustParticles();
	void spawnDustParticle();
	void drawRotatedSquare(float cx, float cy, float size, float rotation, Uint8 alpha);
	
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