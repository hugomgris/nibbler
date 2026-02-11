#include "../incs/SDLTextRenderer.hpp"

SDLTextRenderer::SDLTextRenderer(SDL_Renderer* renderer) 
	: renderer(renderer), mainFont(nullptr), smallFont(nullptr), initialized(false),
	  windowWidth(0), windowHeight(0), cellSize(0), borderOffset(0), square(0) {}

SDLTextRenderer::~SDLTextRenderer() {
	if (mainFont) {
		TTF_CloseFont(mainFont);
		mainFont = nullptr;
	}
	if (smallFont) {
		TTF_CloseFont(smallFont);
		smallFont = nullptr;
	}
}

bool SDLTextRenderer::init(int width, int height, int cell, int border) {
	windowWidth = width;
	windowHeight = height;
	cellSize = cell;
	borderOffset = border;
	square = cellSize;

	smallMode = ((windowWidth / 2) < 900) ? true : false;

	int mainSize = (windowWidth < 1800) ? 34 : 34;
	int smallSize = (windowWidth < 1800) ? 24 : 24;

	// Load main font
	mainFont = TTF_OpenFont("fonts/JetBrainsMono-VariableFont_wght.ttf", mainSize);
	if (!mainFont) {
		// Fallback to system font
		mainFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", mainSize);
		if (!mainFont) {
			std::cerr << "[SDL Text Renderer] Failed to load main font: " << TTF_GetError() << std::endl;
			return false;
		}
	}

	// Load small font
	smallFont = TTF_OpenFont("fonts/JetBrainsMono-VariableFont_wght.ttf", smallSize);
	if (!smallFont) {
		// Fallback to system font
		smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", smallSize);
		if (!smallFont) {
			std::cerr << "[SDL Text Renderer] Failed to load small font: " << TTF_GetError() << std::endl;
			// Use main font as fallback
			smallFont = mainFont;
		}
	}

	initialized = (mainFont != nullptr);
	return initialized;
}

bool SDLTextRenderer::drawText(const std::string& text, int x, int y, int offset, 
								TTF_Font* fontToUse, SDL_Color color, bool centered) {
	if (!fontToUse || !initialized)
		return false;

	SDL_Surface* surface = TTF_RenderUTF8_Blended(fontToUse, text.c_str(), color);
	if (!surface) {
		std::cerr << "[SDL Text Renderer] Failed to render text: " << TTF_GetError() << std::endl;
		return false;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		SDL_FreeSurface(surface);
		std::cerr << "[SDL Text Renderer] Failed to create texture: " << SDL_GetError() << std::endl;
		return false;
	}

	SDL_Rect destRect;
	if (centered) {
		destRect = {
			x - (surface->w / 2),
			y + offset - (surface->h / 2),
			surface->w,
			surface->h
		};
	} else {
		destRect = {x, y + offset, surface->w, surface->h};
	}

	SDL_RenderCopy(renderer, texture, nullptr, &destRect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}

void SDLTextRenderer::drawInstruction(int centerX, int centerY, int& offset,
                                       const std::string& labelText, const std::string& dotText,
                                       bool smallMode, TTF_Font* currentFont) {
	if (!initialized)
		return;

	// text -> white
	SDL_Color textColor = customWhite;
	drawText(labelText, centerX, centerY, offset, currentFont, textColor, true);

	// Dots -> gray
	textColor = customGray;
	drawText(dotText, centerX, centerY, offset, currentFont, textColor, true);

	offset += (smallMode ? 40 : 60);  // instructions line offset
}

void SDLTextRenderer::drawMode(const GameState &state, int centerX, int centerY, int& offset, bool smallMode, TTF_Font* currentFont) {
	if (!initialized)
		return;

	std::string stateA;
	std::string stateB;
	SDL_Color textColor;

	if (!smallMode) {
		offset -= 100;
	}

	switch (state.config.mode) {
		case GameMode::SINGLE:
			stateA = "SINGLE               ";
			stateB = "       - MULTI - VsAI";
			textColor = lightBlue;
			break;

		case GameMode::MULTI:
			stateA = "         MULTI       ";
			stateB = "SINGLE -       - VsAI";
			textColor = goldenYellow;
			break;

		case GameMode::AI:
			stateA = "                 VsAI";
			stateB = "SINGLE - MULTI -     ";
			textColor = lightGreen;
			break;
	}
	drawText(stateA, centerX, centerY, offset, currentFont, textColor, true);
	textColor = customGray;
	drawText(stateB, centerX, centerY, offset, currentFont, textColor, true);

	offset += (smallMode ? 40 : 60);  // instructions line offset
}

void SDLTextRenderer::drawInstructions(const GameState &state, int centerX, int centerY) {
	if (!initialized)
		return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	int offset = smallMode ? square * 2 : square * 6;


	drawMode(state, centerX, centerY, offset, smallMode, currentFont);

	offset += (smallMode ? 40 : 60);
	
	// Enter instruction
	std::string instructionTextA = smallMode ?
		"[ ENTER ]          START" :
		"[ ENTER ]             START";
	std::string instructionTextB = smallMode ?
		"         ··········     " :
		"         ············     ";
	drawInstruction(centerX, centerY, offset, 
	                instructionTextA, instructionTextB, 
	                smallMode, currentFont);
	
	// Mode instruction
	instructionTextA = smallMode ?
		"[ SPACE ]           MODE" :
		"[ SPACE ]              MODE";
	instructionTextB = smallMode ?
		"          ··········    " :
		"          ·············    ";
	drawInstruction(centerX, centerY, offset, 
	                instructionTextA, instructionTextB, 
	                smallMode, currentFont);
	
	// Move instruction
	instructionTextA = smallMode ?
		"[ ↑ ↓ ← → ]         MOVE" :
		"[ ↑ ↓ ← → ]            MOVE";
	instructionTextB = smallMode ?
		"           ·········    " :
		"           ············    ";
	drawInstruction(centerX, centerY, offset, 
	                instructionTextA, instructionTextB, 
	                smallMode, currentFont);

	// Travel instruction
	instructionTextA = smallMode ?
		"[ 1   2   3 ]     TRAVEL" :
		"[ 1   2   3 ]        TRAVEL";
	instructionTextB = smallMode ?
		"    /   /    ·····      " :
		"    /   /    ········      ";
	drawInstruction(centerX, centerY, offset, 
	                instructionTextA, instructionTextB, 
	                smallMode, currentFont);

	// Quit instruction
	instructionTextA = smallMode ?
		"[ Q   ESC ]         QUIT" :
		"[ Q   ESC ]            QUIT";
	instructionTextB = smallMode ?
		"    /      ·········    " :
		"    /      ···········    ";
	drawInstruction(centerX, centerY, offset, 
	                instructionTextA, instructionTextB, 
	                smallMode, currentFont);
}

void SDLTextRenderer::drawScore(const GameState& state, int centerX, int centerY) {
	if (!initialized)
		return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	if (!smallMode) {
		centerY -= 100;
	}

	int yOffset = smallMode ? square * 3 : square * 7;
	
	if (state.config.mode != GameMode::SINGLE && state.snake_B) {
		// MULTIPLAYER
		int lineSpacing = smallMode ? 40 : 60;
		
		// Player 1 Score
		std::string scoreNumA = std::to_string(state.score);
		std::string appleWordA = (state.score == 1) ? "APPLE" : "APPLES";
		
		int wordSpacing = 10;
		
		SDL_Surface* player1Surface = TTF_RenderUTF8_Blended(currentFont, "PLAYER 1", lightBlue);
		SDL_Surface* ateASurface = TTF_RenderUTF8_Blended(currentFont, "ATE", customWhite);
		SDL_Surface* scoreASurface = TTF_RenderUTF8_Blended(currentFont, scoreNumA.c_str(), lightRed);
		SDL_Surface* appleASurface = TTF_RenderUTF8_Blended(currentFont, appleWordA.c_str(), customWhite);
		
		if (!player1Surface || !ateASurface || !scoreASurface || !appleASurface) {
			if (player1Surface) SDL_FreeSurface(player1Surface);
			if (ateASurface) SDL_FreeSurface(ateASurface);
			if (scoreASurface) SDL_FreeSurface(scoreASurface);
			if (appleASurface) SDL_FreeSurface(appleASurface);
			return;
		}
		
		int totalWidthA = player1Surface->w + wordSpacing + ateASurface->w + wordSpacing + 
		                  scoreASurface->w + wordSpacing + appleASurface->w;
		
		int startXA = centerX - (totalWidthA / 2);
		int currentXA = startXA;
		
		// Render Player 1 line
		SDL_Texture* player1Texture = SDL_CreateTextureFromSurface(renderer, player1Surface);
		if (player1Texture) {
			SDL_Rect rect = {currentXA, centerY + yOffset - (player1Surface->h / 2), 
			                 player1Surface->w, player1Surface->h};
			SDL_RenderCopy(renderer, player1Texture, nullptr, &rect);
			SDL_DestroyTexture(player1Texture);
		}
		currentXA += player1Surface->w + wordSpacing;
		SDL_FreeSurface(player1Surface);
		
		SDL_Texture* ateATexture = SDL_CreateTextureFromSurface(renderer, ateASurface);
		if (ateATexture) {
			SDL_Rect rect = {currentXA, centerY + yOffset - (ateASurface->h / 2), 
			                 ateASurface->w, ateASurface->h};
			SDL_RenderCopy(renderer, ateATexture, nullptr, &rect);
			SDL_DestroyTexture(ateATexture);
		}
		currentXA += ateASurface->w + wordSpacing;
		SDL_FreeSurface(ateASurface);
		
		SDL_Texture* scoreATexture = SDL_CreateTextureFromSurface(renderer, scoreASurface);
		if (scoreATexture) {
			SDL_Rect rect = {currentXA, centerY + yOffset - (scoreASurface->h / 2), 
			                 scoreASurface->w, scoreASurface->h};
			SDL_RenderCopy(renderer, scoreATexture, nullptr, &rect);
			SDL_DestroyTexture(scoreATexture);
		}
		currentXA += scoreASurface->w + wordSpacing;
		SDL_FreeSurface(scoreASurface);
		
		SDL_Texture* appleATexture = SDL_CreateTextureFromSurface(renderer, appleASurface);
		if (appleATexture) {
			SDL_Rect rect = {currentXA, centerY + yOffset - (appleASurface->h / 2), 
			                 appleASurface->w, appleASurface->h};
			SDL_RenderCopy(renderer, appleATexture, nullptr, &rect);
			SDL_DestroyTexture(appleATexture);
		}
		SDL_FreeSurface(appleASurface);
		
		// Player 2/AI Score (next line)
		std::string scoreNumB = std::to_string(state.scoreB);
		std::string appleWordB = (state.scoreB == 1) ? "APPLE" : "APPLES";
		
		// Determine label and color based on game mode
		const char* player2Label = (state.config.mode == GameMode::AI) ? "AI" : "PLAYER 2";
		SDL_Color player2Color = (state.config.mode == GameMode::AI) ? lightGreen : goldenYellow;
		
		SDL_Surface* player2Surface = TTF_RenderUTF8_Blended(currentFont, player2Label, player2Color);
		SDL_Surface* ateBSurface = TTF_RenderUTF8_Blended(currentFont, "ATE", customWhite);
		SDL_Surface* scoreBSurface = TTF_RenderUTF8_Blended(currentFont, scoreNumB.c_str(), lightRed);
		SDL_Surface* appleBSurface = TTF_RenderUTF8_Blended(currentFont, appleWordB.c_str(), customWhite);
		
		if (!player2Surface || !ateBSurface || !scoreBSurface || !appleBSurface) {
			if (player2Surface) SDL_FreeSurface(player2Surface);
			if (ateBSurface) SDL_FreeSurface(ateBSurface);
			if (scoreBSurface) SDL_FreeSurface(scoreBSurface);
			if (appleBSurface) SDL_FreeSurface(appleBSurface);
			return;
		}
		
		int totalWidthB = player2Surface->w + wordSpacing + ateBSurface->w + wordSpacing + 
		                  scoreBSurface->w + wordSpacing + appleBSurface->w;
		
		int startXB = centerX - (totalWidthB / 2);
		int currentXB = startXB;
		int yOffsetB = yOffset + lineSpacing;
		
		// Render Player 2 line
		SDL_Texture* player2Texture = SDL_CreateTextureFromSurface(renderer, player2Surface);
		if (player2Texture) {
			SDL_Rect rect = {currentXB, centerY + yOffsetB - (player2Surface->h / 2), 
			                 player2Surface->w, player2Surface->h};
			SDL_RenderCopy(renderer, player2Texture, nullptr, &rect);
			SDL_DestroyTexture(player2Texture);
		}
		currentXB += player2Surface->w + wordSpacing;
		SDL_FreeSurface(player2Surface);
		
		SDL_Texture* ateBTexture = SDL_CreateTextureFromSurface(renderer, ateBSurface);
		if (ateBTexture) {
			SDL_Rect rect = {currentXB, centerY + yOffsetB - (ateBSurface->h / 2), 
			                 ateBSurface->w, ateBSurface->h};
			SDL_RenderCopy(renderer, ateBTexture, nullptr, &rect);
			SDL_DestroyTexture(ateBTexture);
		}
		currentXB += ateBSurface->w + wordSpacing;
		SDL_FreeSurface(ateBSurface);
		
		SDL_Texture* scoreBTexture = SDL_CreateTextureFromSurface(renderer, scoreBSurface);
		if (scoreBTexture) {
			SDL_Rect rect = {currentXB, centerY + yOffsetB - (scoreBSurface->h / 2), 
			                 scoreBSurface->w, scoreBSurface->h};
			SDL_RenderCopy(renderer, scoreBTexture, nullptr, &rect);
			SDL_DestroyTexture(scoreBTexture);
		}
		currentXB += scoreBSurface->w + wordSpacing;
		SDL_FreeSurface(scoreBSurface);
		
		SDL_Texture* appleBTexture = SDL_CreateTextureFromSurface(renderer, appleBSurface);
		if (appleBTexture) {
			SDL_Rect rect = {currentXB, centerY + yOffsetB - (appleBSurface->h / 2), 
			                 appleBSurface->w, appleBSurface->h};
			SDL_RenderCopy(renderer, appleBTexture, nullptr, &rect);
			SDL_DestroyTexture(appleBTexture);
		}
		SDL_FreeSurface(appleBSurface);
		
	} else {
		// SINGLE PLAYER
		std::string scoreNum = std::to_string(state.score);
		std::string appleWord = (state.score == 1) ? "APPLE" : "APPLES";
		
		int wordSpacing = 10;
		
		SDL_Surface* youSurface = TTF_RenderUTF8_Blended(currentFont, "YOU", lightBlue);
		SDL_Surface* ateSurface = TTF_RenderUTF8_Blended(currentFont, "ATE", customWhite);
		SDL_Surface* scoreSurface = TTF_RenderUTF8_Blended(currentFont, scoreNum.c_str(), lightRed);
		SDL_Surface* appleSurface = TTF_RenderUTF8_Blended(currentFont, appleWord.c_str(), customWhite);
		
		if (!youSurface || !ateSurface || !scoreSurface || !appleSurface) {
			if (youSurface) SDL_FreeSurface(youSurface);
			if (ateSurface) SDL_FreeSurface(ateSurface);
			if (scoreSurface) SDL_FreeSurface(scoreSurface);
			if (appleSurface) SDL_FreeSurface(appleSurface);
			return;
		}
		
		int totalWidth = youSurface->w + wordSpacing + ateSurface->w + wordSpacing + 
		                 scoreSurface->w + wordSpacing + appleSurface->w;
		
		int startX = centerX - (totalWidth / 2);
		int currentX = startX;
		
		// "PLAYER 1" - blue
		SDL_Texture* youTexture = SDL_CreateTextureFromSurface(renderer, youSurface);
		if (youTexture) {
			SDL_Rect youRect = {
				currentX,
				centerY + yOffset - (youSurface->h / 2),
				youSurface->w,
				youSurface->h
			};
			SDL_RenderCopy(renderer, youTexture, nullptr, &youRect);
			SDL_DestroyTexture(youTexture);
		}
		currentX += youSurface->w + wordSpacing;
		SDL_FreeSurface(youSurface);
		
		// "ATE" - white
		SDL_Texture* ateTexture = SDL_CreateTextureFromSurface(renderer, ateSurface);
		if (ateTexture) {
			SDL_Rect ateRect = {
				currentX,
				centerY + yOffset - (ateSurface->h / 2),
				ateSurface->w,
				ateSurface->h
			};
			SDL_RenderCopy(renderer, ateTexture, nullptr, &ateRect);
			SDL_DestroyTexture(ateTexture);
		}
		currentX += ateSurface->w + wordSpacing;
		SDL_FreeSurface(ateSurface);
		
		// Score - red
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
		if (scoreTexture) {
			SDL_Rect scoreRect = {
				currentX,
				centerY + yOffset - (scoreSurface->h / 2),
				scoreSurface->w,
				scoreSurface->h
			};
			SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
			SDL_DestroyTexture(scoreTexture);
		}
		currentX += scoreSurface->w + wordSpacing;
		SDL_FreeSurface(scoreSurface);
		
		// "APPLE(S)" - white
		SDL_Texture* appleTexture = SDL_CreateTextureFromSurface(renderer, appleSurface);
		if (appleTexture) {
			SDL_Rect appleRect = {
				currentX,
				centerY + yOffset - (appleSurface->h / 2),
				appleSurface->w,
				appleSurface->h
			};
			SDL_RenderCopy(renderer, appleTexture, nullptr, &appleRect);
			SDL_DestroyTexture(appleTexture);
		}
		SDL_FreeSurface(appleSurface);
	}
}

void SDLTextRenderer::drawWinner(const GameState &state, int centerX, int centerY) {
	enum result {
		A_win,
		B_win,
		Draw
	};
	
	result res;
	std::string winnerText;
	SDL_Color textColor;
	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	int offset = smallMode? - 250 : -400;

	// Determine winner based on who died
	if (state.snake_A.isDead() && state.snake_B->isDead()) {
		// Both died - check scores
		res = Draw;
	} else if (state.snake_A.isDead()) {
		// Only A died
		res = B_win;
	} else if (state.snake_B->isDead()) {
		// Only B died
		res = A_win;
	} else {
		// Neither died yet
		res = Draw;
	}

	if (res == Draw) {
		if (state.score > state.scoreB)
			res = A_win;
		else if (state.scoreB > state.score)
			res = B_win;
	}

	if (state.score == 0 && state.score == state.scoreB)
		res = Draw;

	switch (res) {
		case A_win:
			winnerText = "Player 1 WINS";
			textColor = lightBlue;
			break;

		case B_win:
			if (state.config.mode == GameMode::AI) {
				winnerText = "AI WINS";
				textColor = lightGreen;
			} else {
				winnerText = "Player 2 WINS";
				textColor = goldenYellow;
			}
			break;

		case Draw:
			winnerText = "MATCH ENDED IN DRAW";
			textColor = customWhite;
			break;
	}
	
	drawText(winnerText, centerX, centerY, offset, currentFont, textColor, true);
}

void SDLTextRenderer::drawRetryPrompt(int centerX, int centerY) {
	if (!initialized)
		return;

	TTF_Font* currentFont = smallMode ? smallFont : mainFont;
	int offset = smallMode ? square * 5 : square * 10;

	if (!smallMode) {
		centerY -= 100;
	}
	
	// Retry instructions
	std::string gameoverTextA = smallMode ?
		"[ ENTER ]          RETRY" :
		"[ ENTER ]             RETRY";
	std::string gameoverTextB = smallMode ?
		"         ·········     " :
		"        ·············    ";
	
	drawInstruction(centerX, centerY, offset, 
	                gameoverTextA, gameoverTextB, 
	                smallMode, currentFont);

	// Quit instruction
	gameoverTextA = smallMode ?
		"[ Q   ESC ]         QUIT" :
		"[ Q   ESC ]            QUIT";
	gameoverTextB = smallMode ?
		"    /       ·······     " :
		"    /       ···········    ";
	
	drawInstruction(centerX, centerY, offset, 
	                gameoverTextA, gameoverTextB, 
	                smallMode, currentFont);
}

TTF_Font*	SDLTextRenderer::getMainFont() const { return mainFont; }
TTF_Font*	SDLTextRenderer::getSmallFont() const { return smallFont; }
bool		SDLTextRenderer::isInitialized() const { return initialized; }
