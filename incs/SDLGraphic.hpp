#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "ParticleSystem.hpp"
#include "TextRenderer.hpp"
#include "TitleHandler.hpp"
#include "colors.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <memory>

struct BorderLine {
	float progress;        // 0 = at arena, 1 = at window edge
	float age;
	
	BorderLine() : progress(0.0f), age(0.0f) {}
};

class SDLGraphic : public IGraphic {
	private:
		SDL_Window*										window;
		SDL_Renderer*									renderer;
		int												windowWidth;
		int												windowHeight;
		int												gridWidth;
		int												gridHeight;
		int												cellSize;
		int												square;
		int												sep;
		int												borderOffset;
		std::unique_ptr<ParticleSystem>					particleSystem;
		std::unique_ptr<TextRenderer>					textRenderer;
		std::unique_ptr<TitleHandler>					titleHandler;
		std::vector<BorderLine>							borderLines;
		std::chrono::high_resolution_clock::time_point	lastSpawnTime;
		float											spawnInterval;
		float											animationSpeed;
		bool											enableTunnelEffect;

		// This is needed for explosion particle spawning
		int												lastFoodX;
		int												lastFoodY;
		
		// Snake trail tracking for interpolation -> the lerping of the trail particles, so that they don't look BAD
		float											lastTailX;
		float											lastTailY;
		bool											isFirstFrame;

		// Colors
		static constexpr SDL_Color customWhite{255, 248, 227, 255};	// Off-white
		static constexpr SDL_Color customGray{136, 136, 136, 255};	// Gray
		static constexpr SDL_Color customBlack{23, 23, 23, 255};	// Charcoal black

		static constexpr SDL_Color lightRed{254, 74, 81, 255};
		//static constexpr SDL_Color darkRed{180, 52, 58, 255};

		static constexpr SDL_Color lightBlue{70, 130, 180, 255};
		//static constexpr SDL_Color darkBlue{18, 45, 68, 255};
		
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
		void drawInstructions(int centerX, int centerY);
		void drawRetryText(const GameState &state, int centerX, int centerY);	public:
		SDLGraphic();
		SDLGraphic(const SDLGraphic&) = delete;
		SDLGraphic &operator=(const SDLGraphic&) = delete;
		~SDLGraphic();
		
		void init(int width, int height) override;
		void render(const GameState& state, float deltaTime) override;
		void renderMenu(const GameState &state, float deltaTime) override;
		void renderGameOver(const GameState &state, float deltaTime) override;
		Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new SDLGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}