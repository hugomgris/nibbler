#include "../../incs/TitleHandler.hpp"
#include "../../incs/Renderer.hpp"

TitleHandler::TitleHandler(Renderer& parent) : renderer(parent) {}

void TitleHandler::drawRects(const std::vector<Rectangle>& rects, Color color) {
	for (const auto& rect : rects) {
		DrawRectangleRec(rect, color);
	}
}

void TitleHandler::renderTitle(int centerX, int centerY, int square, int sep, Color white, Color blue, Color red) {
	int totalWidth = (26 * square) + (6 * sep);
	int startX = centerX - (totalWidth / 2);
	
	// n
	std::vector<Rectangle> nRects = {
		{(float)startX, (float)(centerY - (square * 3)), (float)square, (float)(square * 5)},
		{(float)(startX + square), (float)(centerY - (square * 3)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 3)), (float)(centerY - (square * 2)), (float)(square * 2), (float)square},
		{(float)(startX + (square * 4)), (float)(centerY - (square * 1)), (float)square, (float)(square * 3)},
	};
	drawRects(nRects, white);

	// i base
	std::vector<Rectangle> iBaseRects = {
		{(float)(startX + (square * 5) + sep), (float)(centerY - (square * 4)), (float)square, (float)(square * 7)},
		{(float)(startX + (square * 5) + sep), (float)(centerY + (square * 3)), (float)(square * 27), (float)square},
	};
	drawRects(iBaseRects, blue);

	// i dot
	std::vector<Rectangle> iDotRects = {
		{(float)(startX + (square * 5) + sep), (float)(centerY - (square * 6)), (float)square, (float)square},
	};
	drawRects(iDotRects, red);

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
	drawRects(bblerRects, white);
}

void TitleHandler::renderGameOver(int centerX, int centerY, int square, int sep, Color white, Color gray) {
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
	drawRects(gRects, white);

	// a
	std::vector<Rectangle> aRects = {
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 2)), (float)square, (float)(square * 3)},
		{(float)(startX + (square * 9) + (sep)), (float)(centerY - (square * 2)), (float)square, (float)(square * 3)},
		{(float)(startX + (square * 5) + (sep)), (float)(centerY + (square)), (float)(square * 7), (float)square},
	};
	drawRects(aRects, white);

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
	drawRects(mRects, white);

	// e
	std::vector<Rectangle> eRects = {
		{(float)(startX + (square * 21) + (sep * 3)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 21) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 25) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 2)},
		{(float)(startX + (square * 22) + (sep * 3)), (float)(centerY - (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 22) + (sep * 3)), (float)(centerY + (square)), (float)(square * 4), (float)square},
	};
	drawRects(eRects, white);

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
	drawRects(oRects, gray);

	// v
	std::vector<Rectangle> vRects = {
		{(float)(startX + (square * 5) + (sep)), (float)(centerY - (square * 3)), (float)square, (float)(square  * 5)},
		{(float)(startX + (square * 6) + (sep)), (float)(centerY + (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 8) + (sep)), (float)(centerY), (float)square, (float)square},
		{(float)(startX + (square * 9) + (sep)), (float)(centerY - (square * 3)), (float)square, (float)(square  * 4)},
	};
	drawRects(vRects, gray);

	// e (reuse vector)
	eRects.clear();
	eRects = {
		{(float)(startX + (square * 10) + (sep * 2)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 10) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 14) + (sep * 2)), (float)(centerY - (square * 2)), (float)square, (float)(square * 2)},
		{(float)(startX + (square * 11) + (sep * 2)), (float)(centerY - (square)), (float)(square * 3), (float)square},
		{(float)(startX + (square * 11) + (sep * 2)), (float)(centerY + (square)), (float)(square * 4), (float)square},
	};
	drawRects(eRects, gray);

	// r
	std::vector<Rectangle> rRects = {
		{(float)(startX + (square * 15) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)(square * 4)},
		{(float)(startX + (square * 15) + (sep * 3)), (float)(centerY - (square * 3)), (float)(square * 5), (float)square},
		{(float)(startX + (square * 19) + (sep * 3)), (float)(centerY - (square * 2)), (float)square, (float)square},
	};
	drawRects(rRects, gray);
}

// 2D Wrapper methods
void TitleHandler::drawTitle() {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int square = 10;
	int sep = 15;
	
	Color white = {255, 248, 227, 255};
	Color blue = {70, 130, 180, 255};
	Color red = {254, 74, 81, 255};
	
	renderTitle(screenWidth / 2, screenHeight / 2, square, sep, white, blue, red);
}

void TitleHandler::drawGameover() {
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int square = 10;
	int sep = 15;
	
	Color white = {255, 248, 227, 255};
	Color gray = {125, 125, 125, 255};
	
	renderGameOver(screenWidth / 2, screenHeight / 2, square, sep, white, gray);
}
