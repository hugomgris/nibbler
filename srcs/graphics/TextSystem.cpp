#include "../../incs/TextSystem.hpp"
#include <raymath.h>

TextSystem::TextSystem() 
	: fontSize(30), smallFontSize(20), smallMode(false) {
}

TextSystem::~TextSystem() {
	UnloadFont(customFont);
}

void TextSystem::init() {
	loadFont();
	
	int screenWidth = GetScreenWidth();
	smallMode = ((screenWidth / 2) < 900);
}

void TextSystem::loadFont() {
	int codepointCount = 256 - 32 + 8;
	int *codepoints = new int[codepointCount];
	
	for (int i = 0; i < (256 - 32); i++) {
		codepoints[i] = 32 + i;
	}
	
	codepoints[256 - 32 + 0] = 0x2191;  // ↑
	codepoints[256 - 32 + 1] = 0x2193;  // ↓
	codepoints[256 - 32 + 2] = 0x2190;  // ←
	codepoints[256 - 32 + 3] = 0x2192;  // →
	codepoints[256 - 32 + 4] = 0x00B7;  // ·
	codepoints[256 - 32 + 5] = 0x2022;  // •
	codepoints[256 - 32 + 6] = 0x00B7;  // ·
	codepoints[256 - 32 + 7] = 0x002E;  // .
	
	customFont = LoadFontEx("fonts/JetBrainsMono-VariableFont_wght.ttf", 64, codepoints, codepointCount);
	delete[] codepoints;
}

Font& TextSystem::getFont() {
	return customFont;
}

int TextSystem::measureText(const std::string& text, int fontSize) {
	Vector2 textSize = MeasureTextEx(customFont, text.c_str(), fontSize, 1.0f);
	return static_cast<int>(textSize.x);
}

// Custom rectangle drawing based text

void TextSystem::drawRects(const std::vector<Rectangle>& rects, Color color) {
	for (const auto& rect : rects) {
		DrawRectangleRec(rect, color);
	}
}

void TextSystem::drawLogo(int centerX, int centerY, int square, int sep,
                          Color whiteColor, Color blueColor, Color redColor) {
	int totalWidth = (26 * square) + (6 * sep);
	int startX = centerX - (totalWidth / 2);
	
	// n
	std::vector<Rectangle> nRects = {
		{(float)startX, (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(startX + square), (float)(centerY - (square * 3)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 3)), (float)(centerY - (square * 2)), (float)(square * 2), (float)square},
		{(float)(startX + (square * 4)), (float)(centerY - (square * 1)), (float)square, (float)(square * 3)},
	};
	drawRects(nRects, whiteColor);

	// i base
	std::vector<Rectangle> iBaseRects = {
		{(float)(startX + (square * 5) + sep), (float)(centerY - (square * 4)), (float)square, (float)(square * 7)},
		{(float)(startX + (square * 5) + sep), (float)(centerY + (square * 3)), (float)(square * 27), (float)square},
	};
	drawRects(iBaseRects, blueColor);

	// i dot
	std::vector<Rectangle> iDotRects = {
		{(float)(startX + (square * 5) + sep), (float)(centerY - (square * 6)), (float)square, (float)square},
	};
	drawRects(iDotRects, redColor);

	// bbler
	int bStartX = startX + (square * 6) + (sep * 2);
	std::vector<Rectangle> bblerRects = {
		// First 'b'
		{(float)bStartX, (float)(centerY - (square * 6)), (float)square, (float)(square * 8)},
		{(float)(bStartX + square), (float)(centerY - (square * 3)), (float)(square * 4), (float)square},
		{(float)(bStartX + (square * 4)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(bStartX + square), (float)(centerY + square), (float)(square * 3), (float)square},

		// Second 'b'
		{(float)(bStartX + (square * 5) + sep), (float)(centerY - (square * 6)), (float)square, (float)(square * 8)},
		{(float)(bStartX + (square * 6) + sep), (float)(centerY - (square * 3)), (float)(square * 4), (float)square},
		{(float)(bStartX + (square * 9) + sep), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(bStartX + (square * 6) + sep), (float)(centerY + square), (float)(square * 3), (float)square},

		// 'l'
		{(float)(bStartX + (square * 10) + (sep * 2)), (float)(centerY - (square * 6)), (float)square, (float)(square * 8)},

		// 'e'
		{(float)(bStartX + (square * 11) + (sep * 3)), (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(bStartX + (square * 12) + (sep * 3)), (float)(centerY - (square * 3)), (float)(square * 4), (float)square},
		{(float)(bStartX + (square * 15) + (sep * 3)), (float)(centerY - (square * 3)), (float)square, (float)(square * 3)},
		{(float)(bStartX + (square * 12) + (sep * 3)), (float)(centerY - square), (float)(square * 3), (float)square},
		{(float)(bStartX + (square * 12) + (sep * 3)), (float)(centerY + square), (float)(square * 4), (float)square},

		// 'r'
		{(float)(bStartX + (square * 16) + (sep * 4)), (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(bStartX + (square * 17) + (sep * 4)), (float)(centerY - (square * 3)), (float)(square * 4), (float)square},
		{(float)(bStartX + (square * 20) + (sep * 4)), (float)(centerY - (square * 2)), (float)(square * 1), (float)square},
	};
	drawRects(bblerRects, whiteColor);
}

void TextSystem::drawGameOverLogo(int centerX, int centerY, int square, int sep,
                                  Color whiteColor, Color grayColor) {
	int totalWidth = (26 * square) + (3 * sep);
	int startX = centerX - (totalWidth / 2);
	centerY = centerY - (square * 3.5);
	
	// g
	std::vector<Rectangle> gRects = {
		{(float)startX, (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)startX, (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 4)), (float)(centerY - (square * 2)), (float)square, (float)(square * 10)},
		{(float)(startX + (square)), (float)(centerY + (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square)), (float)(centerY + (square * 7)), (float)(square * 3), (float)square},
		{(float)startX, (float)(centerY + (square * 4)), (float)square, (float)(square * 3)},
		{(float)(startX + (square)), (float)(centerY + (square * 6)), (float)square, (float)square},
	};
	drawRects(gRects, whiteColor);

	// a
	std::vector<Rectangle> aRects = {
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 2)), (float)square, (float)(square * 3)},
		{(float)(startX + (square * 9) + (sep)), (float)(centerY - (square * 2)), (float)square, (float)(square * 3)},
		{(float)(startX + (square * 5) + (sep)), (float)(centerY + (square)), (float)(square * 7), (float)square},
	};
	drawRects(aRects, whiteColor);

	// m
	std::vector<Rectangle> mRects = {
		{(float)(startX + (square * 10) + (sep * 2)), (float)(centerY - (square * 3)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 11) + (sep * 2)), (float)(centerY - (square * 3)), (float)(square * 2), (float)square},
		{(float)(startX + (square * 12) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 13) + (sep * 2)), (float)(centerY  + (sep)), (float)square, (float)square},
		{(float)(startX + (square * 14) + (sep * 2)), (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(startX + (square * 15) + (sep * 2)), (float)(centerY - (square * 3)), (float)(square * 2), (float)square},
		{(float)(startX + (square * 16) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 17) + (sep * 2)), (float)(centerY  + (sep)), (float)square, (float)square},
		{(float)(startX + (square * 18) + (sep * 2)), (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(startX + (square * 19) + (sep * 2)), (float)(centerY - (square * 3)), (float)(square * 2), (float)square},
		{(float)(startX + (square * 20) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
	};
	drawRects(mRects, whiteColor);

	// e
	std::vector<Rectangle> eRects = {
		{(float)(startX + (square * 21) + (sep * 3)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 21) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 25) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 2)},
		{(float)(startX + (square * 22) + (sep * 3)), (float)(centerY - (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 22) + (sep * 3)), (float)(centerY + (square)), (float)(square * 4), (float)square},
	};
	drawRects(eRects, whiteColor);

	// over
	centerY = centerY + (square * 5) + (sep);
	startX = startX + (square * 5) + sep;
	
	// o
	std::vector<Rectangle> oRects = {
		{(float)startX, (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)startX, (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 4)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)startX, (float)(centerY + (square)), (float)(square * 4), (float)square},
	};
	drawRects(oRects, grayColor);

	// v
	std::vector<Rectangle> vRects = {
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 3)), (float)square, (float)(square  * 5)},
		{(float)(startX + (square * 6) + (sep)), (float)(centerY + (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 8) + (sep)), (float)(centerY), (float)square, (float)square},
		{(float)(startX + (square * 9) + (sep)), (float)(centerY - (square * 3)), (float)square, (float)(square  * 4)},
	};
	drawRects(vRects, grayColor);

	// e (reuse vector)
	eRects.clear();
	eRects = {
		{(float)(startX + (square * 10) + (sep * 2)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 10) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 14) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 2)},
		{(float)(startX + (square * 11) + (sep * 2)), (float)(centerY - (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 11) + (sep * 2)), (float)(centerY + (square)), (float)(square * 4), (float)square},
	};
	drawRects(eRects, grayColor);

	// r
	std::vector<Rectangle> rRects = {
		{(float)(startX + (square * 15) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 15) + (sep * 3)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 19) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)square},
	};
	drawRects(rRects, grayColor);
}

// Font-based text rendering

void TextSystem::drawText(const std::string& text, int x, int y, int fontSize, 
                          Color color, bool centered) {
	Vector2 textSize = MeasureTextEx(customFont, text.c_str(), fontSize, 1.0f);
	
	int drawX = centered ? x - (textSize.x / 2) : x;
	int drawY = y - (textSize.y / 2);
	
	DrawTextEx(customFont, text.c_str(), (Vector2){(float)drawX, (float)drawY}, fontSize, 1.0f, color);
}

void TextSystem::drawInstruction(int centerX, int centerY, int& offset, 
                                 const std::string& labelText, const std::string& dotText,
                                 Color whiteColor, Color grayColor) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	
	// Draw label in white
	drawText(labelText, centerX, centerY + offset, currentFontSize, whiteColor, true);
	
	// Draw dots in gray
	drawText(dotText, centerX, centerY + offset, currentFontSize, grayColor, true);
	
	offset += (smallMode ? 40 : 60);
}

void TextSystem::drawMode(const GameState& state, int centerX, int centerY, int& offset,
                          Color /*whiteColor*/, Color grayColor) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	
	std::string stateA;
	std::string stateB;
	Color textColor;
	
	switch (state.config.mode) {
		case GameMode::SINGLE:
			stateA = "SINGLE               ";
			stateB = "       - MULTI - VsAI";
			textColor = {70, 130, 180, 255};  // lightBlue
			break;
		case GameMode::MULTI:
			stateA = "         MULTI       ";
			stateB = "SINGLE -       - VsAI";
			textColor = {255, 215, 0, 255};  // goldenYellow
			break;
		case GameMode::AI:
			stateA = "                 VsAI";
			stateB = "SINGLE - MULTI -     ";
			textColor = {144, 238, 144, 255};  // lightGreen
			break;
	}
	
	drawText(stateA, centerX, centerY + offset, currentFontSize, textColor, true);
	drawText(stateB, centerX, centerY + offset, currentFontSize, grayColor, true);
	
	offset += (smallMode ? 40 : 60);
}

void TextSystem::drawInstructions(const GameState& state, int centerX, int centerY,
                                  Color whiteColor, Color grayColor) {
	int square = 10;  // Match menu square size
	int offset = smallMode ? square * 2 : square * 6;
	
	drawMode(state, centerX, centerY, offset, whiteColor, grayColor);
	
	offset += (smallMode ? 40 : 60);
	
	// Enter instruction
	std::string instructionTextA = smallMode ? 
		"[ ENTER ]          START" : 
		"[ ENTER ]             START";
	std::string instructionTextB = smallMode ? 
		"         ··········     " : 
		"         ············     ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB, whiteColor, grayColor);
	
	// Mode instruction
	instructionTextA = smallMode ? 
		"[ SPACE ]           MODE" : 
		"[ SPACE ]              MODE";
	instructionTextB = smallMode ? 
		"          ··········    " : 
		"          ·············    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB, whiteColor, grayColor);
	
	// Move instruction
	instructionTextA = smallMode ? 
		"[ ↑ ↓ ← → ]         MOVE" : 
		"[ ↑ ↓ ← → ]            MOVE";
	instructionTextB = smallMode ? 
		"           ·········    " : 
		"           ············    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB, whiteColor, grayColor);
	
	// Travel instruction
	instructionTextA = smallMode ? 
		"[ 1   2   3 ]     TRAVEL" : 
		"[ 1   2   3 ]        TRAVEL";
	instructionTextB = smallMode ? 
		"    /   /    ·····      " : 
		"    /   /    ········      ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB, whiteColor, grayColor);
	
	// Quit instruction
	instructionTextA = smallMode ? 
		"[ Q   ESC ]         QUIT" : 
		"[ Q   ESC ]            QUIT";
	instructionTextB = smallMode ? 
		"    /      ·········    " : 
		"    /      ···········    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB, whiteColor, grayColor);
}

void TextSystem::drawScore(const GameState& state, int centerX, int centerY,
                           Color whiteColor) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 20 : square * 25;
	
	if (!smallMode) {
		centerY -= 100;
	}
	
	if (state.config.mode != GameMode::SINGLE && state.snake_B) {
		// Player 1
		std::string player1Text = "PLAYER 1";
		std::string ateText = "ATE";
		std::string score1 = std::to_string(state.score);
		std::string apples1 = (state.score == 1) ? "APPLE" : "APPLES";
		
		Color blueColor = {70, 130, 180, 255};
		Color redColor = {254, 74, 81, 255};
		
		Vector2 p1Size = MeasureTextEx(customFont, player1Text.c_str(), currentFontSize, 1.0f);
		Vector2 ateSize = MeasureTextEx(customFont, ateText.c_str(), currentFontSize, 1.0f);
		Vector2 score1Size = MeasureTextEx(customFont, score1.c_str(), currentFontSize, 1.0f);
		Vector2 apples1Size = MeasureTextEx(customFont, apples1.c_str(), currentFontSize, 1.0f);
		
		int wordSpacing = 10;
		int totalWidth1 = p1Size.x + wordSpacing + ateSize.x + wordSpacing + score1Size.x + wordSpacing + apples1Size.x;
		int startX1 = centerX - (totalWidth1 / 2);
		int currentX1 = startX1;
		int y1 = centerY + yOffset;
		
		DrawTextEx(customFont, player1Text.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, blueColor);
		currentX1 += p1Size.x + wordSpacing;
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, whiteColor);
		currentX1 += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score1.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, redColor);
		currentX1 += score1Size.x + wordSpacing;
		DrawTextEx(customFont, apples1.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, whiteColor);
		
		// Player 2
		std::string player2Text = (state.config.mode == GameMode::MULTI) ? "PLAYER 2" : "AI";
		std::string score2 = std::to_string(state.scoreB);
		std::string apples2 = (state.scoreB == 1) ? "APPLE" : "APPLES";
		
		Color yellowColor = {255, 215, 0, 255};
		Color greenColor = {144, 238, 144, 255};
		Color player2Color = (state.config.mode == GameMode::MULTI) ? yellowColor : greenColor;
		
		Vector2 p2Size = MeasureTextEx(customFont, player2Text.c_str(), currentFontSize, 1.0f);
		Vector2 score2Size = MeasureTextEx(customFont, score2.c_str(), currentFontSize, 1.0f);
		Vector2 apples2Size = MeasureTextEx(customFont, apples2.c_str(), currentFontSize, 1.0f);
		
		int lineSpacing = smallMode ? 40 : 60;
		int totalWidth2 = p2Size.x + wordSpacing + ateSize.x + wordSpacing + score2Size.x + wordSpacing + apples2Size.x;
		int startX2 = centerX - (totalWidth2 / 2);
		int currentX2 = startX2;
		int y2 = y1 + lineSpacing;
		
		DrawTextEx(customFont, player2Text.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, player2Color);
		currentX2 += p2Size.x + wordSpacing;
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, whiteColor);
		currentX2 += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score2.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, redColor);
		currentX2 += score2Size.x + wordSpacing;
		DrawTextEx(customFont, apples2.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, whiteColor);
	} else {
		// Single player
		std::string youText = "YOU";
		std::string ateText = "ATE";
		std::string score = std::to_string(state.score);
		std::string apples = (state.score == 1) ? "APPLE" : "APPLES";
		
		Color blueColor = {70, 130, 180, 255};
		Color redColor = {254, 74, 81, 255};
		
		Vector2 youSize = MeasureTextEx(customFont, youText.c_str(), currentFontSize, 1.0f);
		Vector2 ateSize = MeasureTextEx(customFont, ateText.c_str(), currentFontSize, 1.0f);
		Vector2 scoreSize = MeasureTextEx(customFont, score.c_str(), currentFontSize, 1.0f);
		Vector2 applesSize = MeasureTextEx(customFont, apples.c_str(), currentFontSize, 1.0f);
		
		int wordSpacing = 10;
		int totalWidth = youSize.x + wordSpacing + ateSize.x + wordSpacing + scoreSize.x + wordSpacing + applesSize.x;
		int startX = centerX - (totalWidth / 2);
		int currentX = startX;
		int y = centerY + yOffset;
		
		DrawTextEx(customFont, youText.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, blueColor);
		currentX += youSize.x + wordSpacing;
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, whiteColor);
		currentX += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, redColor);
		currentX += scoreSize.x + wordSpacing;
		DrawTextEx(customFont, apples.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, whiteColor);
	}
}

void TextSystem::drawWinner(const GameState& state, int centerX, int centerY,
                            Color whiteColor) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 15 : square * 20;
	
	if (!smallMode) {
		centerY -= 100;
	}
	
	std::string winnerText;
	Color winnerColor;
	
	if (state.config.mode == GameMode::MULTI) {
		if (state.score > state.scoreB) {
			winnerText = "PLAYER 1 WINS!";
			winnerColor = {70, 130, 180, 255};  // blue
		} else if (state.scoreB > state.score) {
			winnerText = "PLAYER 2 WINS!";
			winnerColor = {255, 215, 0, 255};  // yellow
		} else {
			winnerText = "IT'S A TIE!";
			winnerColor = whiteColor;
		}
	} else if (state.config.mode == GameMode::AI) {
		if (state.score > state.scoreB) {
			winnerText = "YOU WIN!";
			winnerColor = {70, 130, 180, 255};  // blue
		} else if (state.scoreB > state.score) {
			winnerText = "AI WINS!";
			winnerColor = {144, 238, 144, 255};  // green
		} else {
			winnerText = "IT'S A TIE!";
			winnerColor = whiteColor;
		}
	}
	
	drawText(winnerText, centerX, centerY + yOffset, currentFontSize, winnerColor, true);
}

void TextSystem::drawRetryPrompt(int centerX, int centerY, Color whiteColor) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 35 : square * 40;
	
	if (!smallMode) {
		centerY -= 100;
	}
	
	std::string retryText = "PRESS ENTER TO PLAY AGAIN";
	drawText(retryText, centerX, centerY + yOffset, currentFontSize, whiteColor, true);
}
