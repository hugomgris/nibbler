#pragma once
#include <raylib.h>
#include "DataStructs.hpp"

// Forward declaration to avoid circular dependency
class RaylibGraphic;

class RaylibTextRenderer {
	private:
		RaylibGraphic&	graphic;
		Font			customFont;
		int				fontSize;
		int				smallFontSize;
		bool			smallMode;
		
		void loadFont();
		void drawText(const std::string& text, int x, int y, int fontSize, Color color, bool centered = true);
		void drawInstruction(int centerX, int centerY, int& offset, const std::string& labelText, const std::string& dotText);
		void drawMode(const GameState &state, int centerX, int centerY, int& offset);

	public:
		RaylibTextRenderer() = delete;
		RaylibTextRenderer(RaylibGraphic& parent);
		RaylibTextRenderer(const RaylibTextRenderer &other) = delete;
		RaylibTextRenderer &operator=(const RaylibTextRenderer &other) = delete;

		~RaylibTextRenderer();
		
		Font& getFont();
		
		void drawInstructions(const GameState &state);
		void drawWinner(const GameState& state);
		void drawRetry(const GameState& state);
		void drawScore(const GameState& state);
};