#pragma once

#include "DataStructs.hpp"
#include <raylib.h>
#include <vector>
#include <string>
#include <iostream>

class TextSystem {
private:
	Font customFont;
	int fontSize;
	int smallFontSize;
	bool smallMode;
	
	// Font loading
	void loadFont();
	
	// Helper for rectangle batch drawing (pixel art)
	void drawRects(const std::vector<Rectangle>& rects, Color color);
	
	// Helper for font-based text
	void drawText(const std::string& text, int x, int y, int fontSize, 
	              Color color, bool centered = true);
	
	// Helper for menu instructions
	void drawInstruction(int centerX, int centerY, int& offset, 
	                     const std::string& labelText, const std::string& dotText,
	                     Color whiteColor, Color grayColor);
	
	// Helper for mode selection display
	void drawMode(const GameState& state, int centerX, int centerY, int& offset,
	              Color whiteColor, Color grayColor);

public:
	TextSystem();
	~TextSystem();
	
	// No copy/assignment
	TextSystem(const TextSystem& other) = delete;
	TextSystem& operator=(const TextSystem& other) = delete;
	
	// Initialize system
	void init();
	
	// ===== PIXEL ART RENDERING (Manual square-based logos) =====
	void drawLogo(int centerX, int centerY, int square, int sep,
	              Color whiteColor, Color blueColor, Color redColor);
	
	void drawGameOverLogo(int centerX, int centerY, int square, int sep,
	                      Color whiteColor, Color grayColor);
	
	// ===== FONT-BASED TEXT RENDERING =====
	void drawInstructions(const GameState& state, int centerX, int centerY,
	                      Color whiteColor, Color grayColor);
	
	void drawWinner(const GameState& state, int centerX, int centerY,
	                Color whiteColor);
	
	void drawScore(const GameState& state, int centerX, int centerY,
	               Color whiteColor);
	
	void drawRetryPrompt(int centerX, int centerY, Color whiteColor);
	
	// ===== UTILITIES =====
	Font& getFont();
	int measureText(const std::string& text, int fontSize);
};
