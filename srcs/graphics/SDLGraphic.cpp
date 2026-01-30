#include "../../incs/SDLGraphic.hpp"
#include <cmath>

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50), borderOffset(0),
	spawnInterval(0.3f), animationSpeed(.5f), enableTunnelEffect(true),
	lastFoodX(-1), lastFoodY(-1),
	lastTailX(-1.0f), lastTailY(-1.0f), isFirstFrame(true) {
	lastSpawnTime = std::chrono::high_resolution_clock::now();
}

SDLGraphic::~SDLGraphic() {
	textRenderer.reset();
	TTF_Quit();
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
	
	// TTF initialization for text rendering
	if (TTF_Init() < 0) {
		std::cerr << "TTF_Init error: " << TTF_GetError() << std::endl;
	}
	
	// Initialize text renderer
	textRenderer = std::make_unique<TextRenderer>(renderer);
	if (!textRenderer->init(windowWidth)) {
		std::cerr << "TextRenderer initialization failed" << std::endl;
	}
	
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

void SDLGraphic::drawInstructions(int centerX, int centerY) {
	if (!textRenderer || !textRenderer->isInitialized()) return;

	bool smallMode = ((windowWidth / 2) < 900);
	TTF_Font *currentFont = smallMode ? textRenderer->getSmallFont() : textRenderer->getMainFont();
	
	int offset = square * 7;
	
	// Enter instructions
	std::string instructionTextA = smallMode ?
		"[ ENTER ]          START" :
		"[ ENTER ]             START";
	std::string instructionTextB = smallMode ?
		"          ········      " :
		"          ···········     ";
	textRenderer->renderInstruction(centerX, centerY, offset, 
	                                instructionTextA, instructionTextB, 
	                                smallMode, currentFont);
	
	// Move instructions
	instructionTextA = smallMode ?
		"[ ↑ ↓ ← → ]         MOVE" :
		"[ ↑ ↓ ← → ]            MOVE";
	instructionTextB = smallMode ?
		"            ·······     " :
		"            ··········     ";
	textRenderer->renderInstruction(centerX, centerY, offset, 
	                                instructionTextA, instructionTextB, 
	                                smallMode, currentFont);

	// Travel instructions
	instructionTextA = smallMode ?
		"[ 1   2   3 ]     TRAVEL" :
		"[ 1   2   3 ]        TRAVEL";
	instructionTextB = smallMode ?
		"    /   /     ···       " :
		"    /   /     ······       ";
	textRenderer->renderInstruction(centerX, centerY, offset, 
	                                instructionTextA, instructionTextB, 
	                                smallMode, currentFont);

	// Quit instructions
	instructionTextA = smallMode ?
		"[ Q   ESC ]         QUIT" :
		"[ Q   ESC ]            QUIT";
	instructionTextB = smallMode ?
		"    /       ·······     " :
		"    /       ··········    ";
	textRenderer->renderInstruction(centerX, centerY, offset, 
	                                instructionTextA, instructionTextB, 
	                                smallMode, currentFont);
}

void SDLGraphic::renderMenu(const GameState& state, float deltaTime) {
	(void)state;
	
	windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	windowHeight = (gridHeight * cellSize) + (2 * borderOffset);

	lastFoodX = -1;
	lastFoodY = -1;

	static int frameCounter = 0;
	if (frameCounter % 111 == 0) {
		particleSystem->spawnSnakeTrail(windowWidth / 2 + (square * 13.2), windowHeight / 2 + (square * 3.2), 1, 0, lightBlue);  // Direction: 0° (moving right), trail goes left
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
	drawInstructions(centerX, centerY);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::drawGameOver(int centerX, int centerY) {
	int totalWidth = (22 * square) + (6 * sep);
	int startX = centerX - (totalWidth / 2);

	// g
	setRenderColor(customWhite);

	std::vector<SDL_Rect> gRects = {
		{startX, centerY - (square * 3), square * 5, square},						// g
		{startX, centerY - (square * 2), square, square * 4},
		{startX + (square * 4), centerY - (square * 2), square, square * 7},
		{startX + square, centerY + (square), square * 3, square},
		{startX, centerY + (square * 4), square * 5, square},
	};

	drawRects(gRects);

	// a
		std::vector<SDL_Rect> aRects = {
		{startX + (square * 5) + (sep), centerY - (square * 3), square * 5, square},						// a
		{startX + (square * 5) + (sep), centerY - (square * 2), square, square * 3},
		{startX + (square * 9) + (sep), centerY - (square * 2), square, square * 3},
		{startX + (square * 5) + (sep), centerY + (square), square * 7, square},
	};

	drawRects(aRects);

	// m
		std::vector<SDL_Rect> mRects = {
		{startX + (square * 10) + (sep * 2), centerY - (square * 3), square, square * 3},						// m
		{startX + (square * 11) + (sep * 2), centerY - (square * 3), square * 3, square},
		{startX + (square * 13) + (sep * 2), centerY - (square * 2), square, square * 3},
		{startX + (square * 14) + (sep * 2), centerY - (square * 2), square, square * 3},
		{startX + (square * 15) + (sep * 2), centerY - (square * 2), square * 3, square},
		{startX + (square * 17) + (sep * 2), centerY - (square), square, square * 3},
	};

	drawRects(mRects);
}

void SDLGraphic::drawRetryText(const GameState &state, int centerX, int centerY) {
	if (!textRenderer || !textRenderer->isInitialized()) return;

	bool smallMode = ((windowWidth / 2) < 900);
	
	// Render score
	textRenderer->renderScore(centerX, centerY, state.score, smallMode);  // TODO: Pass actual score
	
	// Render retry prompt
	textRenderer->renderRetryPrompt(centerX, centerY, smallMode);
}

void SDLGraphic::renderGameOver(const GameState& state, float deltaTime) {
	(void)state;

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
	
	drawGameOver(centerX, centerY);
	drawRetryText(state, centerX, centerY);
	
	SDL_RenderPresent(renderer);
}