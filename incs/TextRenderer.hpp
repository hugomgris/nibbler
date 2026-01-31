#ifndef TEXTRENDERER_HPP
#define TEXTRENDERER_HPP

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include "colors.h"

static constexpr SDL_Color customWhite{255, 248, 227, 255};  // Off-white
static constexpr SDL_Color customGray{136, 136, 136, 255};   // Gray
static constexpr SDL_Color lightRed{254, 74, 81, 255};		 // light red - food
static constexpr SDL_Color lightBlue{70, 130, 180, 255};	// light blue - snake

class TextRenderer {
private:
	SDL_Renderer* renderer;
	TTF_Font* mainFont;
	TTF_Font* smallFont;
	bool initialized;

public:
	TextRenderer(SDL_Renderer* renderer);
	~TextRenderer();

	bool init(int windowWidth);

	bool renderText(const std::string& text, int x, int y, int offset, 
	                TTF_Font* fontToUse, SDL_Color color, bool centered = false);

	void renderInstruction(int centerX, int centerY, int& offset,
	                       const std::string& labelText, const std::string& dotText,
	                       bool smallMode, TTF_Font* currentFont);
	
	void renderInstructions(int centerX, int centerY, bool smallMode, int square);

	void renderScore(int centerX, int centerY, int score, bool smallMode, int square);
	void renderRetryPrompt(int centerX, int centerY, bool smallMode, int square);

	TTF_Font* getMainFont() const { return mainFont; }
	TTF_Font* getSmallFont() const { return smallFont; }
	bool isInitialized() const { return initialized; }
};

#endif
