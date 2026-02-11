#pragma once

#include "Renderer.hpp"
#include <raylib.h>
#include <vector>

// Forward declaration to avoid circular dependency
class Renderer;
class TitleHandler {
	private:
		Renderer& renderer;
		
		// Helper to draw multiple rectangles at once
		void drawRects(const std::vector<Rectangle>& rects, Color color);

	public:
		TitleHandler() = delete;
		TitleHandler(Renderer& parent);
		TitleHandler(const TitleHandler &other) = delete;
		TitleHandler &operator=(const TitleHandler &other) = delete;

		~TitleHandler() = default;
		
		void drawTitle();
		void drawGameover();
		
		// New 2D rendering methods
		void renderTitle(int centerX, int centerY, int square, int sep, Color white, Color blue, Color red);
		void renderGameOver(int centerX, int centerY, int square, int sep, Color white, Color gray);
};