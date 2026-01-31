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
	int mainSize = (windowWidth < 1800) ? 34 : 34;
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

void TextRenderer::renderInstructions(int centerX, int centerY, bool smallMode, int square) {
	if (!initialized) return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	int offset = square * 7;
	
	// Enter instruction
	std::string instructionTextA = smallMode ?
		"[ ENTER ]          START" :
		"[ ENTER ]             START";
	std::string instructionTextB = smallMode ?
		"          ········      " :
		"          ···········     ";
	renderInstruction(centerX, centerY, offset, 
	                  instructionTextA, instructionTextB, 
	                  smallMode, currentFont);
	
	// Move instruction
	instructionTextA = smallMode ?
		"[ ↑ ↓ ← → ]         MOVE" :
		"[ ↑ ↓ ← → ]            MOVE";
	instructionTextB = smallMode ?
		"            ·······     " :
		"            ··········     ";
	renderInstruction(centerX, centerY, offset, 
	                  instructionTextA, instructionTextB, 
	                  smallMode, currentFont);

	// Travel instruction
	instructionTextA = smallMode ?
		"[ 1   2   3 ]     TRAVEL" :
		"[ 1   2   3 ]        TRAVEL";
	instructionTextB = smallMode ?
		"    /   /     ···       " :
		"    /   /     ······       ";
	renderInstruction(centerX, centerY, offset, 
	                  instructionTextA, instructionTextB, 
	                  smallMode, currentFont);

	// Quit instruction
	instructionTextA = smallMode ?
		"[ Q   ESC ]         QUIT" :
		"[ Q   ESC ]            QUIT";
	instructionTextB = smallMode ?
		"    /       ·······     " :
		"    /       ··········    ";
	renderInstruction(centerX, centerY, offset, 
	                  instructionTextA, instructionTextB, 
	                  smallMode, currentFont);
}

void TextRenderer::renderScore(int centerX, int centerY, int score, bool smallMode, int square) {
	if (!initialized) return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	int offset = square * 8;
	
	std::string scoreNum = std::to_string(score);
	std::string appleWord = (score == 1) ? "APPLE" : "APPLES";
	
	int spacing = 10;
	
	// "YOU" - blue
	SDL_Surface* youSurface = TTF_RenderUTF8_Blended(currentFont, "YOU", lightBlue);

	if (youSurface) {
		int youWidth = youSurface->w;
		SDL_FreeSurface(youSurface);
		
		// "ATE" - white
		SDL_Surface* ateSurface = TTF_RenderUTF8_Blended(currentFont, "ATE", customWhite);
		int ateWidth = ateSurface ? ateSurface->w : 0;
		if (ateSurface) SDL_FreeSurface(ateSurface);
		
		// Score number - red
		SDL_Surface* scoreSurface = TTF_RenderUTF8_Blended(currentFont, scoreNum.c_str(), lightRed);
		int scoreWidth = scoreSurface ? scoreSurface->w : 0;
		if (scoreSurface) SDL_FreeSurface(scoreSurface);
		
		// Apple word - white
		SDL_Surface* appleSurface = TTF_RenderUTF8_Blended(currentFont, appleWord.c_str(), customWhite);
		int appleWidth = appleSurface ? appleSurface->w : 0;
		if (appleSurface) SDL_FreeSurface(appleSurface);
		
		int totalWidth = youWidth + spacing + ateWidth + spacing + scoreWidth + spacing + appleWidth;
		
		int startX = centerX - (totalWidth / 2);
		
		renderText("YOU", startX, centerY, offset, currentFont, lightBlue, false);
		startX += youWidth + spacing;
		
		renderText("ATE", startX, centerY, offset, currentFont, customWhite, false);
		startX += ateWidth + spacing;
		
		renderText(scoreNum, startX, centerY, offset, currentFont, lightRed, false);
		startX += scoreWidth + spacing;
		
		renderText(appleWord, startX, centerY, offset, currentFont, customWhite, false);
	}
}

void TextRenderer::renderRetryPrompt(int centerX, int centerY, bool smallMode, int square) {
	if (!initialized) return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;

	int offset = square * 11;
	
	// Retry instructions
	std::string gameoverTextA = smallMode ?
		"[ ENTER ]          RETRY" :
		"[ ENTER ]             RETRY";
	std::string gameoverTextB = smallMode ?
		"          ·······      " :
		"          ··········     ";
	
	renderInstruction(centerX, centerY, offset, 
	                  gameoverTextA, gameoverTextB, 
	                  smallMode, currentFont);

	// Quit instruction
	gameoverTextA = smallMode ?
		"[ Q   ESC ]         QUIT" :
		"[ Q   ESC ]            QUIT";
	gameoverTextB = smallMode ?
		"    /       ·······     " :
		"    /       ··········    ";
	
	renderInstruction(centerX, centerY, offset, 
	                  gameoverTextA, gameoverTextB, 
	                  smallMode, currentFont);
}
