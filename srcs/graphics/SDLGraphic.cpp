#include "../../incs/SDLGraphic.hpp"

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50), borderOffset(0),
	spawnInterval(0.3f), animationSpeed(.5f), enableTunnelEffect(true) {
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

	// Store grid dimensions
	gridWidth = width;
	gridHeight = height;
	
	// Border offset is 1 * cellSize on each side
	borderOffset = 1 * cellSize;
	
	// Window size = game arena + 2*borderOffset (on each side)
	int windowWidth = (width * cellSize) + (2 * borderOffset);
	int windowHeight = (height * cellSize) + (2 * borderOffset);
	
	window = SDL_CreateWindow(
		"Nibbler - SDL2",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_SHOWN
	);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	std::cout << BRED << "[SDL2] Initialized: " << width << "x" << height << RESET << std::endl;
}

void SDLGraphic::setRenderColor(SDL_Color color, bool customAlpha, Uint8 alphaValue) {
	if (customAlpha) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alphaValue);
	} else {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	}
	
}
	
void SDLGraphic::render(const GameState& state) {
	static auto lastFrameTime = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> deltaTime = now - lastFrameTime;
	lastFrameTime = now;

	updateTunnelEffect(deltaTime.count());

	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	renderTunnelEffect();
	
	// Draw snake -> refactor into its own function?
	setRenderColor(lightBlue);
	for (int i = 0; i < state.snake->getLength(); ++i) {
		SDL_Rect rect = {
			borderOffset + (state.snake->getSegments()[i].x * cellSize),
			borderOffset + (state.snake->getSegments()[i].y * cellSize),
			cellSize,
			cellSize
		};
		SDL_RenderFillRect(renderer, &rect);
	}
	
	// Draw food -> refactor into its own function?
	setRenderColor(lightRed);
	SDL_Rect foodRect = {
		borderOffset + (state.food->getPosition().x * cellSize),
		borderOffset + (state.food->getPosition().y * cellSize),
		cellSize,
		cellSize
	};
	SDL_RenderFillRect(renderer, &foodRect);

	drawBorder(cellSize);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::drawBorder(int thickness) {
	setRenderColor(customWhite);
	
	int innerX = borderOffset;
	int innerY = borderOffset;
	int innerW = gridWidth * cellSize;
	int innerH = gridHeight * cellSize;
	
	// Draw 4 filled semi-transparent borders
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
		
		Uint8 alpha = static_cast<Uint8>(line.progress * 150);

		int lineWidth = 1;

		int arenaX = borderOffset;
		int arenaY = borderOffset;
		int arenaW = gridWidth * cellSize;
		int arenaH = gridHeight * cellSize;

		setRenderColor(customWhite, true, alpha);

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