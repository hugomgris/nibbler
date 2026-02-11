#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include "colors.h"
#include "DataStructs.hpp"
#include "Snake.hpp"

static constexpr SDL_Color customWhite{255, 248, 227, 255};  // Off-white
static constexpr SDL_Color customGray{136, 136, 136, 255};   // Gray
static constexpr SDL_Color lightRed{254, 74, 81, 255};		 // light red - food
static constexpr SDL_Color lightBlue{70, 130, 180, 255};	// light blue - snake
static constexpr SDL_Color lightGreen{144, 238, 144, 255};	// Light green
static constexpr SDL_Color goldenYellow{255, 215, 0, 255};	// Golden yellow

class SDLTextRenderer {
	private:
		SDL_Renderer*	renderer;
		TTF_Font*		mainFont;
		TTF_Font*		smallFont;
		bool			smallMode;
		bool			initialized;
		int				windowWidth;
		int				windowHeight;
		int				cellSize;
		int				borderOffset;
		int				square;

		bool drawText(const std::string& text, int x, int y, int offset, 
					TTF_Font* fontToUse, SDL_Color color, bool centered = false);

		void drawInstruction(int centerX, int centerY, int& offset,
							const std::string& labelText, const std::string& dotText,
							bool smallMode, TTF_Font* currentFont);

		void drawMode(const GameState &state, int centerX, int centerY, int& offset, bool smallMode, TTF_Font* currentFont);

	public:
		SDLTextRenderer();
		SDLTextRenderer(SDL_Renderer* renderer);
		SDLTextRenderer(const SDLTextRenderer &other) = delete;
		SDLTextRenderer &operator=(const SDLTextRenderer &other) = delete;

		~SDLTextRenderer();

		bool init(int windowWidth, int windowHeight, int cellSize, int borderOffset);

		void drawInstructions(const GameState& state, int centerX, int centerY);
		void drawWinner(const GameState &state, int centerX, int centerY);
		void drawScore(const GameState& state, int centerX, int centerY);
		void drawRetryPrompt(int centerX, int centerY);

		TTF_Font* getMainFont() const;
		TTF_Font* getSmallFont() const;
		bool isInitialized() const;
};