#include "../incs/TextRenderer.hpp"

TextRenderer::TextRenderer(SDL_Renderer* renderer) 
	: renderer(renderer), mainFont(nullptr), smallFont(nullptr), initialized(false) {
}

TextRenderer::~TextRenderer() {
	if (mainFont) {
		TTF_CloseFont(mainFont);
		mainFont = nullptr;
	}
	if (smallFont) {
		TTF_CloseFont(smallFont);
		smallFont = nullptr;
	}
}

bool TextRenderer::init(int windowWidth) {
	int mainSize = (windowWidth < 1800) ? 40 : 40;
	int smallSize = (windowWidth < 1800) ? 24 : 24;

	// Load main font
	mainFont = TTF_OpenFont("fonts/JetBrainsMono-VariableFont_wght.ttf", mainSize);
	if (!mainFont) {
		// Fallback to system font
		mainFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", mainSize);
		if (!mainFont) {
			std::cerr << "Main font loading error: " << TTF_GetError() << std::endl;
			return false;
		}
	}

	// Load small font
	smallFont = TTF_OpenFont("fonts/JetBrainsMono-VariableFont_wght.ttf", smallSize);
	if (!smallFont) {
		// Fallback to system font
		smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", smallSize);
		if (!smallFont) {
			std::cerr << "Small font loading error: " << TTF_GetError() << std::endl;
			// Use main font as fallback for small font
			smallFont = mainFont;
		}
	}

	initialized = (mainFont != nullptr);
	return initialized;
}

bool TextRenderer::renderText(const std::string& text, int x, int y, int offset, 
                               TTF_Font* fontToUse, SDL_Color color, bool centered) {
	if (!fontToUse || !initialized) return false;

	SDL_Surface* surface = TTF_RenderUTF8_Blended(fontToUse, text.c_str(), color);
	if (!surface) {
		std::cerr << "Text render error: " << TTF_GetError() << std::endl;
		return false;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		SDL_FreeSurface(surface);
		return false;
	}

	SDL_Rect destRect;
	if (centered) {
		destRect = {
			x - (surface->w / 2),
			y - (surface->h / 2) + offset,
			surface->w,
			surface->h
		};
	} else {
		destRect = { x, y + offset, surface->w, surface->h };
	}

	SDL_RenderCopy(renderer, texture, nullptr, &destRect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}

void TextRenderer::renderInstruction(int centerX, int centerY, int& offset,
                                     const std::string& labelText, const std::string& dotText,
                                     bool smallMode, TTF_Font* currentFont) {
	if (!initialized) return;

	// text -> white
	SDL_Color textColor = customWhite;
	renderText(labelText, centerX, centerY, offset, currentFont, textColor, true);

	// Dots -> gray
	textColor = customGray;
	renderText(dotText, centerX, centerY, offset, currentFont, textColor, true);

	offset += (smallMode ? 40 : 90);  // isntructions line offset
}

void TextRenderer::renderScore(int centerX, int centerY, int score, bool smallMode) {
	if (!initialized) return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	
	std::string scoreText = "SCORE: " + std::to_string(score);
	int offset = 70;
	
	renderText(scoreText, centerX, centerY, offset, currentFont, customWhite, true);
}

void TextRenderer::renderRetryPrompt(int centerX, int centerY, bool smallMode) {
	if (!initialized) return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	
	std::string promptTextA = smallMode ?
		"[ ENTER ]          RETRY" :
		"[ ENTER ]             RETRY";
	std::string promptTextB = smallMode ?
		"          ·······      " :
		"          ··········     ";
	
	int offset = 100;
	
	// Render label
	renderText(promptTextA, centerX, centerY, offset, currentFont, customWhite, true);
	
	// Render dots
	renderText(promptTextB, centerX, centerY, offset, currentFont, customGray, true);
}
