#pragma once

#include "DataStructs.hpp"
#include "Renderer.hpp"
#include <raylib.h>

class TextRenderer {
	private:
		Renderer&		renderer;
		Font			customFont;
		int				fontSize;
		int				smallFontSize;
		bool			smallMode;
		
		void loadFont();
		void drawText(const std::string& text, int x, int y, int fontSize, Color color, bool centered = true);
		void drawInstruction(int centerX, int centerY, int& offset, const std::string& labelText, const std::string& dotText);
		void drawMode(const GameState &state, int centerX, int centerY, int& offset);

	public:
		TextRenderer() = delete;
		TextRenderer(Renderer& parent);
		TextRenderer(const TextRenderer &other) = delete;
		TextRenderer &operator=(const TextRenderer &other) = delete;

		~TextRenderer();
		
		Font& getFont();
		
		void drawInstructions(const GameState &state);
		void drawWinner(const GameState& state);
		void drawRetry(const GameState& state);
		void drawScore(const GameState& state);
};