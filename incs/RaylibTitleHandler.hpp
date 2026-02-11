#pragma once
#include <raylib.h>
#include <vector>

// Forward declaration to avoid circular dependency
class RaylibGraphic;

class RaylibTitleHandler {
	private:
		RaylibGraphic& graphic;
		
		// Helper to draw multiple rectangles at once
		void drawRects(const std::vector<Rectangle>& rects, Color color);

	public:
		RaylibTitleHandler() = delete;
		RaylibTitleHandler(RaylibGraphic& parent);
		RaylibTitleHandler(const RaylibTitleHandler &other) = delete;
		RaylibTitleHandler &operator=(const RaylibTitleHandler &other) = delete;

		~RaylibTitleHandler() = default;
		
		void drawTitle();
		void drawGameover();
		
		// New 2D rendering methods
		void renderTitle(int centerX, int centerY, int square, int sep, Color white, Color blue, Color red);
		void renderGameOver(int centerX, int centerY, int square, int sep, Color white, Color gray);
};