#include "../../incs/SDLGraphic.hpp"
#include <cmath>

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50), borderOffset(0),
	spawnInterval(0.3f), animationSpeed(.5f), enableTunnelEffect(true),
	lastFoodX(-1), lastFoodY(-1),
	lastTailX(-1.0f), lastTailY(-1.0f), isFirstFrame(true) {
	lastSpawnTime = std::chrono::high_resolution_clock::now();
}

SDLGraphic::~SDLGraphic() {
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << BRED << "[SDL2] Destroyed" << RESET << std::endl;
}

void SDLGraphic::init(int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
		return;
	}

	gridWidth = width;
	gridHeight = height;
	
	borderOffset = 1 * cellSize;
	
	// Window size = game arena + 2*borderOffset (on each side)
	int windowWidth = (width * cellSize) + (2 * borderOffset);
	int windowHeight = (height * cellSize) + (2 * borderOffset);

	if ((windowWidth / 2) < 900) {
		sep = 20;
		square = 20;
	} else {
		sep = 45;
		square = 45;
	}
	
	window = SDL_CreateWindow(
		"Nibbler - SDL2",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN
	);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	// Initialize particle system
	particleSystem = std::make_unique<ParticleSystem>(renderer, width, height, cellSize, borderOffset);
	
	borderLines.reserve(100);
	
	std::cout << BRED << "[SDL2] Initialized: " << width << "x" << height << RESET << std::endl;
}

void SDLGraphic::setRenderColor(SDL_Color color, bool customAlpha, Uint8 alphaValue) {
	if (customAlpha) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alphaValue);
	} else {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}
	
}
	
void SDLGraphic::render(const GameState& state, float deltaTime) {

	updateTunnelEffect(deltaTime);
	particleSystem->update(deltaTime);

	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	renderTunnelEffect();
	particleSystem->render();
	
	drawSnake(state);
	drawFood(state);

	drawBorder(cellSize);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::drawSnake(const GameState &state) {
	setRenderColor(lightBlue);
	for (int i = 0; i < state.snake.getLength(); ++i) {
		SDL_Rect rect = {
			borderOffset + (state.snake.getSegments()[i].x * cellSize),
			borderOffset + (state.snake.getSegments()[i].y * cellSize),
			cellSize,
			cellSize
		};
		SDL_RenderFillRect(renderer, &rect);
		
		if (i == state.snake.getLength() - 1 && state.snake.getLength() > 1) {
			float tailX = borderOffset + (state.snake.getSegments()[i].x * cellSize) + (cellSize / 2.0f);
			float tailY = borderOffset + (state.snake.getSegments()[i].y * cellSize) + (cellSize / 2.0f);
			
			Vec2 tail = state.snake.getSegments()[i];
			Vec2 beforeTail = state.snake.getSegments()[i - 1];
			float direction = 0.0f;
			
			if (tail.x > beforeTail.x) direction = 0.0f;		// Moving right
			else if (tail.x < beforeTail.x) direction = 180.0f;	// Moving left
			else if (tail.y > beforeTail.y) direction = 90.0f;  // Moving down
			else if (tail.y < beforeTail.y) direction = 270.0f; // Moving up
			
			if (!isFirstFrame && (lastTailX != tailX || lastTailY != tailY)) {
				float dx = tailX - lastTailX;
				float dy = tailY - lastTailY;
				float distance = sqrtf(dx * dx + dy * dy);
				
				int steps = static_cast<int>(distance / 15.0f) + 1;
				
				for (int step = 0; step < steps; ++step) {
					float t = static_cast<float>(step) / static_cast<float>(steps);
					float interpX = lastTailX + (dx * t);
					float interpY = lastTailY + (dy * t);
					
					particleSystem->spawnSnakeTrail(interpX - 10.0f, interpY - 10.0f, 1, direction, lightBlue);
				}
			}
			
			lastTailX = tailX;
			lastTailY = tailY;
			isFirstFrame = false;
		}
	}
}

void SDLGraphic::drawFood(const GameState &state) {
	int currentFoodX = state.food.getPosition().x;
	int currentFoodY = state.food.getPosition().y;
	
	if (lastFoodX != -1 && (lastFoodX != currentFoodX || lastFoodY != currentFoodY)) {
		float explosionX = borderOffset + (lastFoodX * cellSize) + (cellSize / 2.0f);
		float explosionY = borderOffset + (lastFoodY * cellSize) + (cellSize / 2.0f);
		particleSystem->spawnExplosion(explosionX, explosionY, 20);  // 20 particles per explosion
	}
	
	lastFoodX = currentFoodX;
	lastFoodY = currentFoodY;
	
	setRenderColor(lightRed);
	SDL_Rect foodRect = {
		borderOffset + (currentFoodX * cellSize),
		borderOffset + (currentFoodY * cellSize),
		cellSize,
		cellSize
	};
	SDL_RenderFillRect(renderer, &foodRect);
}


void SDLGraphic::drawBorder(int thickness) {
	setRenderColor(customWhite);
	
	int innerX = borderOffset;
	int innerY = borderOffset;
	int innerW = gridWidth * cellSize;
	int innerH = gridHeight * cellSize;
	
	// splitting the border into 4 filled rectangles because this is my life now
	SDL_Rect top    = {innerX - thickness, innerY - thickness, innerW + (2 * thickness), thickness};
	SDL_Rect bottom = {innerX - thickness, innerY + innerH, innerW + (2 * thickness), thickness};
	SDL_Rect left   = {innerX - thickness, innerY, thickness, innerH};
	SDL_Rect right  = {innerX + innerW, innerY, thickness, innerH};
	
	SDL_RenderFillRect(renderer, &top);
	SDL_RenderFillRect(renderer, &bottom);
	SDL_RenderFillRect(renderer, &left);
	SDL_RenderFillRect(renderer, &right);
}
	
Input SDLGraphic::pollInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return Input::Quit;
		
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_UP:		return Input::Up;
				case SDLK_DOWN:		return Input::Down;
				case SDLK_LEFT:		return Input::Left;
				case SDLK_RIGHT:	return Input::Right;
				case SDLK_q:		return Input::Quit;
				case SDLK_ESCAPE:	return Input::Quit;
				case SDLK_1:		return Input::SwitchLib1;
				case SDLK_2:		return Input::SwitchLib2;
				case SDLK_3:		return Input::SwitchLib3;
				case SDLK_SPACE:	return Input::Pause;
				case SDLK_RETURN:	return Input::Enter;
				case SDLK_KP_ENTER:	return Input::Enter;
			}
		}
	}
	return Input::None;
}

float SDLGraphic::easeInQuad(float t) {
	return t * t;
}

void SDLGraphic::updateTunnelEffect(float deltaTime) {
	if (!enableTunnelEffect) return;

	for (auto& line : borderLines) {
		line.age += deltaTime * animationSpeed;
		line.progress = easeInQuad(line.age);
	}

	borderLines.erase(
		std::remove_if(borderLines.begin(), borderLines.end(),
			[](const BorderLine& line) { return line.progress >= 1.0f; }),
		borderLines.end()
	);

	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = now - lastSpawnTime;
	
	if (elapsed.count() >= spawnInterval) {
		borderLines.push_back(BorderLine());
		lastSpawnTime = now;
	}
}

void SDLGraphic::renderTunnelEffect() {
	if (!enableTunnelEffect || borderLines.empty()) return;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	int startOffset = -2 * cellSize;
	int endOffset = 0;
	int travelDistance = endOffset - startOffset;

	for (const auto& line : borderLines) {
		int currentOffset = startOffset + static_cast<int>(line.progress * travelDistance);
		
		Uint8 alpha = static_cast<Uint8>(line.progress * 255);

		int lineWidth = 1;

		int arenaX = borderOffset;
		int arenaY = borderOffset;
		int arenaW = gridWidth * cellSize;
		int arenaH = gridHeight * cellSize;

		setRenderColor(lightBlue, true, alpha);

		// Top border
		SDL_Rect top = {
			arenaX - currentOffset,
			arenaY - currentOffset,
			arenaW + (2 * currentOffset),
			lineWidth
		};

		// Bottom border
		SDL_Rect bottom = {
			arenaX - currentOffset,
			arenaY + arenaH + currentOffset - lineWidth,
			arenaW + (2 * currentOffset),
			lineWidth
		};

		// Left border
		SDL_Rect left = {
			arenaX - currentOffset,
			arenaY - currentOffset,
			lineWidth,
			arenaH + (2 * currentOffset)
		};

		// Right border
		SDL_Rect right = {
			arenaX + arenaW + currentOffset - lineWidth,
			arenaY - currentOffset,
			lineWidth,
			arenaH + (2 * currentOffset)
		};

		SDL_RenderFillRect(renderer, &top);
		SDL_RenderFillRect(renderer, &bottom);
		SDL_RenderFillRect(renderer, &left);
		SDL_RenderFillRect(renderer, &right);
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void SDLGraphic::drawRects(const std::vector<SDL_Rect>& rects) {
	for (const auto& rect : rects) {
		SDL_RenderFillRect(renderer, &rect);
	}
}

void SDLGraphic::drawTitle(int centerX, int centerY) {
	int totalWidth = (26 * square) + (6 * sep);
	int startX = centerX - (totalWidth / 2);
	
	//n
	setRenderColor(customWhite);

	std::vector<SDL_Rect> nRects = {
		{startX, centerY - (square * 3), square, square * 5},						// n - left vertical
		{startX + square, centerY - (square * 3), square * 3, square},				// n - top horizontal
		{startX + (square * 3), centerY - (square * 2), square * 2, square},		// n - middle diagonal
		{startX + (square * 4), centerY - (square * 1), square, square * 3},		// n - right vertical
	};

	drawRects(nRects);

	// i base
	setRenderColor(lightBlue);

	std::vector<SDL_Rect> iBaseRects = {
		{startX + (square * 5) + sep, centerY - (square * 4), square, square * 7},			// i - left
		{startX + (square * 5) + sep, centerY + (square * 3), square * 23, square},			// i - base
	};

	drawRects(iBaseRects);

	// i dot
	setRenderColor(lightRed);

	std::vector<SDL_Rect> iDotRects = {
		{startX + (square * 5) + sep, centerY - (square * 6), square, square},  // i - dot
	};

	drawRects(iDotRects);

	// bbler
	setRenderColor(customWhite);

	int bStartX = startX + (square * 6) + (sep * 2);
	
	std::vector<SDL_Rect> bblerRects = {
		// First 'b'
		{bStartX, centerY - (square * 6), square, square * 8},								// b - stem
		{bStartX + square, centerY - (square * 3), square * 4, square},						// b - base, top
		{bStartX + (square * 4), centerY - (square * 2), square, square * 4},				// b - base, right
		{bStartX + square, centerY + square, square * 3, square},							// b - base, bottom

		// Second 'b'
		{bStartX + (square * 5) + sep, centerY - (square * 6), square, square * 8},			// b - stem
		{bStartX + (square * 6) + sep, centerY - (square * 3), square * 4, square},			// b - base, top
		{bStartX + (square * 9) + sep, centerY - (square * 2), square, square * 4},			// b - base, right
		{bStartX + (square * 6) + sep, centerY + square, square * 3, square},				// b - base, bottom

		// 'l'
		{bStartX + (square * 10) + (sep * 2), centerY - (square * 6), square, square * 8},	// l

		// 'e'
		{bStartX + (square * 11) + (sep * 3), centerY - (square * 3), square, square * 5},	// e - left
		{bStartX + (square * 12) + (sep * 3), centerY - (square * 3), square * 4, square},	// e - top
		{bStartX + (square * 15) + (sep * 3), centerY - (square * 3), square, square * 3},	// e - right
		{bStartX + (square * 12) + (sep * 3), centerY - square, square * 3, square},		// e - mid
		{bStartX + (square * 12) + (sep * 3), centerY + square, square * 4, square},		// e - bot

		// 'r'
		{bStartX + (square * 16) + (sep * 4), centerY - (square * 3), square, square * 5},	// r - stem
		{bStartX + (square * 17) + (sep * 4), centerY - (square * 3), square * 3, square},	// r - side
	};

	drawRects(bblerRects);
}

void SDLGraphic::renderMenu(const GameState& state, float deltaTime) {
	(void)state;
	
	windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	windowHeight = (gridHeight * cellSize) + (2 * borderOffset);

	lastFoodX = -1;
	lastFoodY = -1;

	static int frameCounter = 0;
	if (frameCounter % 111 == 0) {
		particleSystem->spawnSnakeTrail(windowWidth / 2 + (square * 13), windowHeight / 2 + (square * 3), 1, 0, lightBlue);  // Direction: 0° (moving right), trail goes left
	}
	frameCounter++;

	// Update animations
	updateTunnelEffect(deltaTime);
	particleSystem->update(deltaTime);
	
	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	// Render animations
	renderTunnelEffect();
	particleSystem->render();
	
	drawBorder(cellSize);
	
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;

	drawTitle(centerX, centerY);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::renderGameOver(const GameState& state, float deltaTime) {
	(void)state;  // Will be used when we add text rendering for score

	// Update animations
	updateTunnelEffect(deltaTime);
	particleSystem->update(deltaTime);
	
	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	// Render animations
	renderTunnelEffect();
	particleSystem->render();
	
	
	drawBorder(cellSize);
	
	int windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	int windowHeight = (gridHeight * cellSize) + (2 * borderOffset);
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;
	
	setRenderColor(lightRed);
	
	// GAME
	std::vector<SDL_Rect> gameRects = {
		// G
		{centerX - 120, centerY - 60, 15, 60},
		{centerX - 120, centerY - 60, 50, 15},
		{centerX - 120, centerY - 15, 50, 15},
		{centerX - 55, centerY - 40, 15, 25},
		{centerX - 85, centerY - 40, 30, 15},
		// A
		{centerX - 35, centerY - 60, 15, 60},
		{centerX - 35, centerY - 60, 40, 15},
		{centerX - 35, centerY - 40, 40, 15},
		{centerX + 5, centerY - 60, 15, 60},
		// M
		{centerX + 30, centerY - 60, 15, 60},
		{centerX + 30, centerY - 60, 15, 30},
		{centerX + 55, centerY - 45, 15, 45},
		{centerX + 80, centerY - 60, 15, 30},
		{centerX + 80, centerY - 60, 15, 60},
		// E
		{centerX + 105, centerY - 60, 15, 60},
		{centerX + 105, centerY - 60, 40, 15},
		{centerX + 105, centerY - 40, 35, 15},
		{centerX + 105, centerY - 15, 40, 15}
	};
	drawRects(gameRects);
	
	// OVER
	int overY = centerY + 5;
	std::vector<SDL_Rect> overRects = {
		// O
		{centerX - 60, overY, 15, 40},
		{centerX - 60, overY, 40, 15},
		{centerX - 60, overY + 25, 40, 15},
		{centerX - 25, overY, 15, 40},
		// V
		{centerX, overY, 15, 30},
		{centerX + 15, overY + 25, 15, 15},
		{centerX + 30, overY, 15, 30},
		// E
		{centerX + 55, overY, 15, 40},
		{centerX + 55, overY, 30, 15},
		{centerX + 55, overY + 12, 25, 15},
		{centerX + 55, overY + 25, 30, 15},
		// R
		{centerX + 95, overY, 15, 40},
		{centerX + 95, overY, 30, 15},
		{centerX + 95, overY + 12, 30, 15},
		{centerX + 120, overY, 15, 12},
		{centerX + 115, overY + 25, 15, 15}
	};
	drawRects(overRects);
	
	// Score display placeholder
	setRenderColor(customWhite);
	int scoreY = centerY + 70;
	for (int i = 0; i < 25; i++) {
		SDL_Rect dot = {centerX - 100 + (i * 8), scoreY, 5, 5};
		SDL_RenderFillRect(renderer, &dot);
	}
	
	// Enter to restart placeholder
	int textY = centerY + 100;
	for (int i = 0; i < 40; i++) {
		SDL_Rect dot = {centerX - 160 + (i * 8), textY, 5, 5};
		SDL_RenderFillRect(renderer, &dot);
	}
	
	SDL_RenderPresent(renderer);
}