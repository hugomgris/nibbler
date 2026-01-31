#include "../incs/TitleHandler.hpp"

TitleHandler::TitleHandler(SDL_Renderer* renderer) : renderer(renderer) {
}

void TitleHandler::drawRects(const std::vector<SDL_Rect>& rects, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	for (const auto& rect : rects) {
		SDL_RenderFillRect(renderer, &rect);
	}
}

void TitleHandler::renderTitle(int centerX, int centerY, int square, int sep, SDL_Color white, SDL_Color blue, SDL_Color red) {
	int totalWidth = (26 * square) + (6 * sep);
	int startX = centerX - (totalWidth / 2);
	
	// n
	std::vector<SDL_Rect> nRects = {
		{startX, centerY - (square * 3), square, square * 5},
		{startX + square, centerY - (square * 3), square * 3, square},
		{startX + (square * 3), centerY - (square * 2), square * 2, square},
		{startX + (square * 4), centerY - (square * 1), square, square * 3},
	};
	drawRects(nRects, white);

	// i base
	std::vector<SDL_Rect> iBaseRects = {
		{startX + (square * 5) + sep, centerY - (square * 4), square, square * 7},
		{startX + (square * 5) + sep, centerY + (square * 3), square * 23, square},
	};
	drawRects(iBaseRects, blue);

	// i dot
	std::vector<SDL_Rect> iDotRects = {
		{startX + (square * 5) + sep, centerY - (square * 6), square, square},
	};
	drawRects(iDotRects, red);

	// bbler
	int bStartX = startX + (square * 6) + (sep * 2);
	std::vector<SDL_Rect> bblerRects = {
		// First 'b'
		{bStartX, centerY - (square * 6), square, square * 8},
		{bStartX + square, centerY - (square * 3), square * 4, square},
		{bStartX + (square * 4), centerY - (square * 2), square, square * 4},
		{bStartX + square, centerY + square, square * 3, square},

		// Second 'b'
		{bStartX + (square * 5) + sep, centerY - (square * 6), square, square * 8},
		{bStartX + (square * 6) + sep, centerY - (square * 3), square * 4, square},
		{bStartX + (square * 9) + sep, centerY - (square * 2), square, square * 4},
		{bStartX + (square * 6) + sep, centerY + square, square * 3, square},

		// 'l'
		{bStartX + (square * 10) + (sep * 2), centerY - (square * 6), square, square * 8},

		// 'e'
		{bStartX + (square * 11) + (sep * 3), centerY - (square * 3), square, square * 5},
		{bStartX + (square * 12) + (sep * 3), centerY - (square * 3), square * 4, square},
		{bStartX + (square * 15) + (sep * 3), centerY - (square * 3), square, square * 3},
		{bStartX + (square * 12) + (sep * 3), centerY - square, square * 3, square},
		{bStartX + (square * 12) + (sep * 3), centerY + square, square * 4, square},

		// 'r'
		{bStartX + (square * 16) + (sep * 4), centerY - (square * 3), square, square * 5},
		{bStartX + (square * 17) + (sep * 4), centerY - (square * 3), square * 3, square},
	};
	drawRects(bblerRects, white);
}

void TitleHandler::renderGameOver(int centerX, int centerY, int square, int sep, SDL_Color white) {
	int totalWidth = (26 * square) + (3 * sep);
	int startX = centerX - (totalWidth / 2);
	centerY = centerY - (square * 2.5);
	
	// g
	std::vector<SDL_Rect> gRects = {
		{startX, centerY - (square * 3), square * 5, square},
		{startX, centerY - (square * 2), square, square * 4},
		{startX + (square * 4), centerY - (square * 2), square, square * 10},
		{startX + (square), centerY + (square), square * 3, square},
		{startX + (square), centerY + (square * 7), square * 3, square},
		{startX, centerY + (square * 4), square, square * 3},
		{startX + (square), centerY + (square * 6), square, square},
	};
	drawRects(gRects, white);

	// a
	std::vector<SDL_Rect> aRects = {
		{startX + (square * 5) + (sep), centerY - (square * 3), square * 5, square},
		{startX + (square * 5) + (sep), centerY - (square * 2), square, square * 3},
		{startX + (square * 9) + (sep), centerY - (square * 2), square, square * 3},
		{startX + (square * 5) + (sep), centerY + (square), square * 7, square},
	};
	drawRects(aRects, white);

	// m
	std::vector<SDL_Rect> mRects = {
		{startX + (square * 10) + (sep * 2), centerY - (square * 3), square, square * 4},
		{startX + (square * 11) + (sep * 2), centerY - (square * 3), square * 2, square},
		{startX + (square * 12) + (sep * 2), centerY - (square * 2), square, square * 4},
		{startX + (square * 13) + (sep * 2), centerY  + (sep), square, square},
		{startX + (square * 14) + (sep * 2), centerY - (square * 3), square, square * 5},
		{startX + (square * 15) + (sep * 2), centerY - (square * 3), square * 2, square},
		{startX + (square * 16) + (sep * 2), centerY - (square * 2), square, square * 4},
		{startX + (square * 17) + (sep * 2), centerY  + (sep), square, square},
		{startX + (square * 18) + (sep * 2), centerY - (square * 3), square, square * 5},
		{startX + (square * 19) + (sep * 2), centerY - (square * 3), square * 2, square},
		{startX + (square * 20) + (sep * 2), centerY - (square * 2), square, square * 4},
	};
	drawRects(mRects, white);

	// e
	std::vector<SDL_Rect> eRects = {
		{startX + (square * 21) + (sep * 3), centerY - (square * 3), square * 5, square},
		{startX + (square * 21) + (sep * 3), centerY - (square * 2), square, square * 4},
		{startX + (square * 25) + (sep * 3), centerY - (square * 2), square, square * 2},
		{startX + (square * 22) + (sep * 3), centerY - (square), square * 3, square},
		{startX + (square * 22) + (sep * 3), centerY + (square), square * 4, square},
	};
	drawRects(eRects, white);

	// over
	centerY = centerY + (square * 5) + (sep);
	startX = startX + (square * 5) + sep;
	
	// o
	std::vector<SDL_Rect> oRects = {
		{startX, centerY - (square * 3), square * 5, square},
		{startX, centerY - (square * 2), square, square * 4},
		{startX + (square * 4), centerY - (square * 2), square, square * 4},
		{startX, centerY + (square), square * 4, square},
	};
	drawRects(oRects, white);

	// v
	std::vector<SDL_Rect> vRects = {
		{startX + (square * 5) + (sep), centerY - (square * 3), square, square  * 5},
		{startX + (square * 6) + (sep), centerY + (square), square * 3, square},
		{startX + (square * 8) + (sep), centerY, square, square},
		{startX + (square * 9) + (sep), centerY - (square * 3), square, square  * 4},
	};
	drawRects(vRects, white);

	// e (reuse vector)
	eRects.clear();
	eRects = {
		{startX + (square * 10) + (sep * 2), centerY - (square * 3), square * 5, square},
		{startX + (square * 10) + (sep * 2), centerY - (square * 2), square, square * 4},
		{startX + (square * 14) + (sep * 2), centerY - (square * 2), square, square * 2},
		{startX + (square * 11) + (sep * 2), centerY - (square), square * 3, square},
		{startX + (square * 11) + (sep * 2), centerY + (square), square * 4, square},
	};
	drawRects(eRects, white);

	// r
	std::vector<SDL_Rect> rRects = {
		{startX + (square * 15) + (sep * 3), centerY - (square * 2), square, square * 4},
		{startX + (square * 15) + (sep * 3), centerY - (square * 3), square * 5, square},
		{startX + (square * 19) + (sep * 3), centerY - (square * 2), square, square},
	};
	drawRects(rRects, white);
}
