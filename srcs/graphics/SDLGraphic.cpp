#include "../../incs/SDLGraphic.hpp"
#include <cmath>

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50), borderOffset(0),
	spawnInterval(0.3f), animationSpeed(.5f), enableTunnelEffect(true),
	maxDustDensity(50), dustSpawnInterval(0.1f), dustSpawnTimer(0.0f),
	dustMinSize(2.0f), dustMaxSize(15.0f), dustMinLifetime(3.0f), dustMaxLifetime(5.0f),
	explosionMinSize(1.0f), explosionMaxSize(50.0f),
	lastFoodX(-1), lastFoodY(-1) {
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
	
void SDLGraphic::render(const GameState& state, float deltaTime) {

	updateTunnelEffect(deltaTime);
	updateParticles(deltaTime);

	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	renderTunnelEffect();
	renderParticles();
	
	drawSnake(state);
	drawFood(state);

	drawBorder(cellSize);
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::drawSnake(const GameState &state) {
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
}

void SDLGraphic::drawFood(const GameState &state) {
	int currentFoodX = state.food->getPosition().x;
	int currentFoodY = state.food->getPosition().y;
	
	// Check if food position has changed (food was eaten)
	if (lastFoodX != -1 && (lastFoodX != currentFoodX || lastFoodY != currentFoodY)) {
		// Spawn explosion at old food position
		float explosionX = borderOffset + (lastFoodX * cellSize) + (cellSize / 2.0f);
		float explosionY = borderOffset + (lastFoodY * cellSize) + (cellSize / 2.0f);
		spawnExplosion(explosionX, explosionY, 20);  // 20 particles per explosion
	}
	
	// Update tracked position
	lastFoodX = currentFoodX;
	lastFoodY = currentFoodY;
	
	// Draw food
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
				case SDLK_SPACE:	return Input::Pause;
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

void SDLGraphic::updateParticles(float deltaTime) {
	dustSpawnTimer += deltaTime;
	if (dustSpawnTimer >= dustSpawnInterval) {
		spawnDustParticle();
		dustSpawnTimer = 0.0f;
	}
	
	// Update all particles
	for (auto& particle : particles) {
		particle.age += deltaTime;
		particle.rotation += particle.rotationSpeed * deltaTime;
		
		// This only applies to explosions, i.e. the only ones with values for vx/vy
		particle.x += particle.vx * deltaTime;
		particle.y += particle.vy * deltaTime;
		
		// Shrinking 
		float progress = particle.age / particle.lifetime;
		particle.currentSize = particle.initialSize * (1.0f - progress) + 1.0f * progress;
	}
	
	// Dead particle removal
	particles.erase(
		std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.age >= p.lifetime; }),
		particles.end()
	);
}

void SDLGraphic::renderParticles() {
	for (const auto& particle : particles) {
		// Fade out
		float progress = particle.age / particle.lifetime;
		Uint8 alpha;
		
		// Alpha handling based on type
		if (particle.type == ParticleType::Dust) {
			alpha = static_cast<Uint8>((1.0f - progress) * 120);
		} else {  // Explosion
			alpha = static_cast<Uint8>((1.0f - progress) * 200);
		}
		
		drawRotatedSquare(particle.x, particle.y, particle.currentSize, particle.rotation, particle.color, alpha);
	}
}

void SDLGraphic::spawnDustParticle() {
	int dustCount = 0;
	for (const auto& p : particles) {
		if (p.type == ParticleType::Dust) dustCount++;
	}
	if (dustCount >= maxDustDensity) return;
	
	int arenaX = borderOffset;
	int arenaY = borderOffset;
	int arenaW = gridWidth * cellSize;
	int arenaH = gridHeight * cellSize;
	
	float x = arenaX + static_cast<float>(rand() % arenaW);
	float y = arenaY + static_cast<float>(rand() % arenaH);
	
	particles.emplace_back(x, y, dustMinSize, dustMaxSize, dustMinLifetime, dustMaxLifetime);
}

void SDLGraphic::spawnExplosion(float x, float y, int count) {
	for (int i = 0; i < count; i++) {
		// Random outward velocity
		float angle = (rand() % 360) * 3.14159f / 180.0f;
		float speed = 50.0f + (rand() % 150);  // 50-200 pixels/sec
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		SDL_Color explosionColor = lightRed;
		
		particles.emplace_back(x, y, explosionMinSize, explosionMaxSize, 0.5f, 1.0f, vx, vy, explosionColor);
	}
}

void SDLGraphic::drawRotatedSquare(float cx, float cy, float size, float rotation, SDL_Color color, Uint8 alpha) {
	// Rotation -> Radians
	float rad = rotation * 3.14159f / 180.0f;
	float halfSize = size / 2.0f;
	
	// 4 corners of the square (centered at origin)
	float corners[4][2] = {
		{-halfSize, -halfSize},  // Top-left
		{ halfSize, -halfSize},  // Top-right
		{ halfSize,  halfSize},  // Bottom-right
		{-halfSize,  halfSize}   // Bottom-left
	};
	
	Sint16 vx[4], vy[4];
	for (int i = 0; i < 4; i++) {
		float x = corners[i][0];
		float y = corners[i][1];
		
		float rotatedX = x * cosf(rad) - y * sinf(rad);
		float rotatedY = x * sinf(rad) + y * cosf(rad);
		
		vx[i] = static_cast<Sint16>(cx + rotatedX);
		vy[i] = static_cast<Sint16>(cy + rotatedY);
	}
	
	SDL_Vertex vertices[4];
	for (int i = 0; i < 4; i++) {
		vertices[i].position.x = static_cast<float>(vx[i]);
		vertices[i].position.y = static_cast<float>(vy[i]);
		vertices[i].color = {color.r, color.g, color.b, alpha};
		vertices[i].tex_coord = {0, 0};
	}
	
	int indices[6] = {0, 1, 2, 0, 2, 3};
	
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
}