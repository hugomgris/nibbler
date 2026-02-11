#include "../../incs/SDLGraphic.hpp"
#include <cmath>

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50), borderOffset(0),
	spawnInterval(0.3f), animationSpeed(.5f), enableTunnelEffect(true), smallMode(false),
	lastFoodX(-1), lastFoodY(-1) {
	lastSpawnTime = std::chrono::high_resolution_clock::now();
	smallMode = ((windowWidth / 2) < 900) ? true : false;
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
		sep = 40;
		square = 40;
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
	textRenderer = std::make_unique<SDLTextRenderer>(renderer);
	if (!textRenderer->init(windowWidth, windowHeight, cellSize, borderOffset)) {
		std::cerr << "TextRenderer initialization failed" << std::endl;
	}
	
	// Initialize title handler
	titleHandler = std::make_unique<SDLTitleHandler>(renderer);
	
	// Initialize particle system
	particleSystem = std::make_unique<SDLParticleSystem>(renderer, width, height, cellSize, borderOffset);
	
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
	
	drawSnake(state.snake_A, snakeATail, lightBlue);
	if (state.config.mode == GameMode::MULTI && state.snake_B)
		drawSnake(*(state.snake_B), snakeBTail, goldenYellow);
	else if (state.config.mode == GameMode::AI && state.snake_B)
		drawSnake(*(state.snake_B), snakeBTail, lightGreen);
	drawFood(state);

	drawBorder(cellSize);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::drawSnake(const Snake &snake, SnakeTailState &tailState, const SDL_Color &color) {
	setRenderColor(color);
	for (int i = 0; i < snake.getLength(); ++i) {
		SDL_Rect rect = {
			borderOffset + (snake.getSegments()[i].x * cellSize),
			borderOffset + (snake.getSegments()[i].y * cellSize),
			cellSize,
			cellSize
		};
		SDL_RenderFillRect(renderer, &rect);
		
		if (i == snake.getLength() - 1 && snake.getLength() > 1) {
			float tailX = borderOffset + (snake.getSegments()[i].x * cellSize) + (cellSize / 2.0f);
			float tailY = borderOffset + (snake.getSegments()[i].y * cellSize) + (cellSize / 2.0f);
			
			Vec2 tail = snake.getSegments()[i];
			Vec2 beforeTail = snake.getSegments()[i - 1];
			float direction = 0.0f;
			
			if (tail.x > beforeTail.x) direction = 0.0f;		// Moving right
			else if (tail.x < beforeTail.x) direction = 180.0f;	// Moving left
			else if (tail.y > beforeTail.y) direction = 90.0f;  // Moving down
			else if (tail.y < beforeTail.y) direction = 270.0f; // Moving up
			
			if (!tailState.isFirstFrame && (tailState.lastX != tailX || tailState.lastY != tailY)) {
				float dx = tailX - tailState.lastX;
				float dy = tailY - tailState.lastY;
				float distance = sqrtf(dx * dx + dy * dy);
				
				int steps = static_cast<int>(distance / 15.0f) + 1;
				
				for (int step = 0; step < steps; ++step) {
					float t = static_cast<float>(step) / static_cast<float>(steps);
					float interpX = tailState.lastX + (dx * t);
					float interpY = tailState.lastY + (dy * t);
					
					particleSystem->spawnSnakeTrail(interpX - 10.0f, interpY - 10.0f, 1, direction, color);
				}
			}
			
			tailState.lastX = tailX;
			tailState.lastY = tailY;
			tailState.isFirstFrame = false;
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
				case SDLK_UP:		return Input::Up_A;
				case SDLK_DOWN:		return Input::Down_A;
				case SDLK_LEFT:		return Input::Left_A;
				case SDLK_RIGHT:	return Input::Right_A;
				case SDLK_w:		return Input::Up_B;
				case SDLK_s:		return Input::Down_B;
				case SDLK_a:		return Input::Left_B;
				case SDLK_d:		return Input::Right_B;
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

void SDLGraphic::renderMenu(const GameState& state, float deltaTime) {
	windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	windowHeight = (gridHeight * cellSize) + (2 * borderOffset);

	lastFoodX = -1;
	lastFoodY = -1;

	static int frameCounter = 0;
	if (frameCounter % 111 == 0) {
		particleSystem->spawnSnakeTrail(windowWidth / 2 + (square * 17.2), windowHeight / 2 + (square * 3.2), 1, 0, lightBlue);
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

	titleHandler->renderTitle(centerX, centerY, square, sep, customWhite, lightBlue, lightRed);
	textRenderer->drawInstructions(state, centerX, centerY);
	
	SDL_RenderPresent(renderer);
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
	
	if (state.config.mode != GameMode::SINGLE) {
		textRenderer->drawWinner(state, centerX, centerY);	
	}

	titleHandler->renderGameOver(centerX, centerY, square, sep, customWhite, customWhite);
	textRenderer->drawScore(state, centerX, centerY);
	textRenderer->drawRetryPrompt(centerX, centerY);
	
	SDL_RenderPresent(renderer);
}

extern "C" IGraphic* createGraphic() {
	return new SDLGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}