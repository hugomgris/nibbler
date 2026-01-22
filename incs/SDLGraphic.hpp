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

enum class ParticleType {
	Dust,
	Explosion
};

struct Particle {
	float			x, y;				// Center position
	float			vx, vy;				// Neded for explosion, maybe something else/more in the future
	float			rotation;          
	float			rotationSpeed;     
	float			initialSize;
	float			currentSize;
	float			lifetime;
	float			age;
	ParticleType	type;
	SDL_Color		color;
	
	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime)
		: x(px), y(py), vx(0), vy(0), age(0.0f), type(ParticleType::Dust), color({ 255, 248, 227, 255}) {
		initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
		currentSize = initialSize;
		lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
		rotation = static_cast<float>(rand() % 360);
		rotationSpeed = -30.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 60.0f;  // -30 to +30 deg/s
	}

	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime, 
			float velocityX, float velocityY, SDL_Color particleColor)
		: x(px), y(py), vx(velocityX), vy(velocityY), age(0.0f), type(ParticleType::Explosion), color(particleColor) {
		initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
		currentSize = initialSize;
		lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
		rotation = static_cast<float>(rand() % 360);
		rotationSpeed = -50.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;  // -50 to +50 deg/s for explosions
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
		std::vector<BorderLine>							borderLines;
		std::chrono::high_resolution_clock::time_point	lastSpawnTime;
		float											spawnInterval;
		float											animationSpeed;
		bool											enableTunnelEffect;

		// Particle system
		std::vector<Particle>	particles;
		int						maxDustDensity;
		float					dustSpawnInterval;
		float					dustSpawnTimer;
		float					dustMinSize;
		float					dustMaxSize;
		float					dustMinLifetime;
		float dustMaxLifetime;

		float					explosionMinSize;
		float					explosionMaxSize;
		
		// This is needed for explosion particle spawning
		int	lastFoodX;
		int	lastFoodY;

		// Colors
		SDL_Color	customWhite = { 255, 248, 227, 255};	// Off-white
		SDL_Color	customBlack = { 23, 23, 23, 255};		// Charcoal black

		SDL_Color	lightRed = { 254, 74, 81, 255 };
		//SDL_Color darkRed = { 180, 52, 58, 255 };

		SDL_Color	lightBlue = { 70, 130, 180, 255 };
		//SDL_Color darkBlue = { 18, 45, 68, 255 };
		
		// Helper function to set render color from SDL_Color
		void setRenderColor(SDL_Color color, bool customAlpha = false, Uint8 alphaValue = 255);
		
		// Tunnel effect helper functions
		void updateTunnelEffect(float deltaTime);
		void renderTunnelEffect();
		float easeInQuad(float t);

		// Drawing functions
		void drawSnake(const GameState &state);
		void drawFood(const GameState &state);
		void drawBorder(int thickness);

		// Particle system helpers
		void updateParticles(float deltaTime);
		void renderParticles();
		void spawnDustParticle();
		void spawnExplosion(float x, float y, int count);
		void drawRotatedSquare(float cx, float cy, float size, float rotation, SDL_Color color, Uint8 alpha);
		
	public:
		SDLGraphic();
		SDLGraphic(const SDLGraphic&) = delete;
		SDLGraphic &operator=(const SDLGraphic&) = delete;
		~SDLGraphic();
		
		void init(int width, int height) override;
		void render(const GameState& state, float deltaTime) override;
		void renderMenu(const GameState &state) override;
		void renderGameOver(const GameState &state) override;
		Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new SDLGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}