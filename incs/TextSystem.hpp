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
	
	// Helpers
	void drawRects(const std::vector<Rectangle>& rects, Color color);

	void drawText(const std::string& text, int x, int y, int fontSize, 
	              Color color, bool centered = true);
	
	void drawInstruction(int centerX, int centerY, int& offset, 
	                     const std::string& labelText, const std::string& dotText,
	                     Color whiteColor, Color grayColor);
	
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
	
	// custom font access
	void drawLogo(int centerX, int centerY, int square, int sep,
	              Color whiteColor, Color blueColor, Color redColor);
	
	void drawGameOverLogo(int centerX, int centerY, int square, int sep,
	                      Color whiteColor, Color grayColor);
	
	// font based text rendering
	void drawInstructions(const GameState& state, int centerX, int centerY,
	                      Color whiteColor, Color grayColor);
	
	void drawWinner(const GameState& state, int centerX, int centerY,
	                Color whiteColor);
	
	void drawScore(const GameState& state, int centerX, int centerY,
	               Color whiteColor);
	
	void drawRetryPrompt(int centerX, int centerY, Color whiteColor);
	
	// utilities
	Font& getFont();
	int measureText(const std::string& text, int fontSize);
};
