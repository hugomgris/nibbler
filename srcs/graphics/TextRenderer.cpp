#include "../../incs/TextRenderer.hpp"
#include "../../incs/Renderer3D.hpp"
#include <raymath.h>

TextRenderer::TextRenderer(Renderer& parent) 
	: renderer(parent), fontSize(34), smallFontSize(24), smallMode(false) {
	loadFont();
	
	int screenWidth = GetScreenWidth();
	smallMode = ((screenWidth / 2) < 900);
}

TextRenderer::~TextRenderer() {
	UnloadFont(customFont);
}

void TextRenderer::loadFont() {
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

Font& TextRenderer::getFont() {
	return customFont;
}

void TextRenderer::drawText(const std::string& text, int x, int y, int fontSize, Color color, bool centered) {
	Vector2 textSize = MeasureTextEx(customFont, text.c_str(), fontSize, 1.0f);
	
	int drawX = centered ? x - (textSize.x / 2) : x;
	int drawY = y - (textSize.y / 2);
	
	DrawTextEx(customFont, text.c_str(), (Vector2){(float)drawX, (float)drawY}, fontSize, 1.0f, color);
}

void TextRenderer::drawInstruction(int centerX, int centerY, int& offset, 
										const std::string& labelText, const std::string& dotText) {
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	
	// Draw label in white
	drawText(labelText, centerX, centerY + offset, currentFontSize, renderer.customWhite, true);
	
	// Draw dots in gray
	drawText(dotText, centerX, centerY + offset, currentFontSize, renderer.customGray, true);
	
	offset += (smallMode ? 40 : 60);
}

void TextRenderer::drawMode(const GameState &state, int centerX, int centerY, int& offset) {
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
	drawText(stateB, centerX, centerY + offset, currentFontSize, renderer.customGray, true);
	
	offset += (smallMode ? 40 : 60);
}

void TextRenderer::drawInstructions(const GameState &state) {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;
	
	int square = 10;  // Match menu square size
	int offset = smallMode ? square * 2 : square * 6;
	
	drawMode(state, centerX, centerY, offset);
	
	offset += (smallMode ? 40 : 60);
	
	// Enter instruction
	std::string instructionTextA = smallMode ? 
		"[ ENTER ]          START" : 
		"[ ENTER ]             START";
	std::string instructionTextB = smallMode ? 
		"         ··········     " : 
		"         ············     ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB);
	
	// Mode instruction
	instructionTextA = smallMode ? 
		"[ SPACE ]           MODE" : 
		"[ SPACE ]              MODE";
	instructionTextB = smallMode ? 
		"          ··········    " : 
		"          ·············    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB);
	
	// Move instruction
	instructionTextA = smallMode ? 
		"[ ↑ ↓ ← → ]         MOVE" : 
		"[ ↑ ↓ ← → ]            MOVE";
	instructionTextB = smallMode ? 
		"           ·········    " : 
		"           ············    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB);
	
	// Travel instruction
	instructionTextA = smallMode ? 
		"[ 1   2   3 ]     TRAVEL" : 
		"[ 1   2   3 ]        TRAVEL";
	instructionTextB = smallMode ? 
		"    /   /    ·····      " : 
		"    /   /    ········      ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB);
	
	// Quit instruction
	instructionTextA = smallMode ? 
		"[ Q   ESC ]         QUIT" : 
		"[ Q   ESC ]            QUIT";
	instructionTextB = smallMode ? 
		"    /      ·········    " : 
		"    /      ···········    ";
	drawInstruction(centerX, centerY, offset, instructionTextA, instructionTextB);
}

void TextRenderer::drawScore(const GameState& state) {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;
	
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 20 : square * 25;  // Push below winner text
	
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
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, renderer.customWhite);
		currentX1 += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score1.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, redColor);
		currentX1 += score1Size.x + wordSpacing;
		DrawTextEx(customFont, apples1.c_str(), (Vector2){(float)currentX1, (float)y1}, currentFontSize, 1.0f, renderer.customWhite);
		
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
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, renderer.customWhite);
		currentX2 += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score2.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, redColor);
		currentX2 += score2Size.x + wordSpacing;
		DrawTextEx(customFont, apples2.c_str(), (Vector2){(float)currentX2, (float)y2}, currentFontSize, 1.0f, renderer.customWhite);
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
		DrawTextEx(customFont, ateText.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, renderer.customWhite);
		currentX += ateSize.x + wordSpacing;
		DrawTextEx(customFont, score.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, redColor);
		currentX += scoreSize.x + wordSpacing;
		DrawTextEx(customFont, apples.c_str(), (Vector2){(float)currentX, (float)y}, currentFontSize, 1.0f, renderer.customWhite);
	}
}

void TextRenderer::drawWinner(const GameState& state) {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;
	
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 15 : square * 20;  // Push below game over logo
	
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
			winnerColor = renderer.customWhite;
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
			winnerColor = renderer.customWhite;
		}
	}
	
	drawText(winnerText, centerX, centerY + yOffset, currentFontSize, winnerColor, true);
}

void TextRenderer::drawRetry(const GameState& /*state*/) {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int centerX = screenWidth / 2;
	int centerY = screenHeight / 2;
	
	int currentFontSize = smallMode ? smallFontSize : fontSize;
	int square = 10;
	int yOffset = smallMode ? square * 35 : square * 40;  // Push to bottom of screen
	
	if (!smallMode) {
		centerY -= 100;
	}
	
	std::string retryText = "PRESS ENTER TO PLAY AGAIN";
	drawText(retryText, centerX, centerY + yOffset, currentFontSize, renderer.customWhite, true);
}
