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
	
	if (lastFoodX != -1 && (lastFoodX != currentFoodX || lastFoodY != currentFoodY)) {
		float explosionX = borderOffset + (lastFoodX * cellSize) + (cellSize / 2.0f);
		float explosionY = borderOffset + (lastFoodY * cellSize) + (cellSize / 2.0f);
		spawnExplosion(explosionX, explosionY, 20);  // 20 particles per explosion
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

void SDLGraphic::renderMenu(const GameState& state) {
	(void)state;
	
	lastFoodX = -1;
	lastFoodY = -1;
	
	particles.clear();
	borderLines.clear();
	
	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	drawBorder(cellSize);
	
	int windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	int windowHeight = (gridHeight * cellSize) + (2 * borderOffset);
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;
	
	setRenderColor(lightBlue);
	
	// N
	SDL_Rect n1 = {centerX - 150, centerY - 80, 15, 60};
	SDL_Rect n2 = {centerX - 150, centerY - 80, 40, 15};
	SDL_Rect n3 = {centerX - 115, centerY - 65, 15, 60};
	
	// I
	SDL_Rect i1 = {centerX - 90, centerY - 80, 15, 60};
	
	// B
	SDL_Rect b1 = {centerX - 65, centerY - 80, 15, 60};
	SDL_Rect b2 = {centerX - 65, centerY - 80, 35, 15};
	SDL_Rect b3 = {centerX - 65, centerY - 50, 35, 15};
	SDL_Rect b4 = {centerX - 65, centerY - 35, 35, 15};
	SDL_Rect b5 = {centerX - 35, centerY - 65, 15, 15};
	SDL_Rect b6 = {centerX - 35, centerY - 20, 15, 15};
	
	// B
	SDL_Rect b7 = {centerX - 10, centerY - 80, 15, 60};
	SDL_Rect b8 = {centerX - 10, centerY - 80, 35, 15};
	SDL_Rect b9 = {centerX - 10, centerY - 50, 35, 15};
	SDL_Rect b10 = {centerX - 10, centerY - 35, 35, 15};
	SDL_Rect b11 = {centerX + 20, centerY - 65, 15, 15};
	SDL_Rect b12 = {centerX + 20, centerY - 20, 15, 15};
	
	// L
	SDL_Rect l1 = {centerX + 45, centerY - 80, 15, 60};
	SDL_Rect l2 = {centerX + 45, centerY - 35, 40, 15};
	
	// E
	SDL_Rect e1 = {centerX + 95, centerY - 80, 15, 60};
	SDL_Rect e2 = {centerX + 95, centerY - 80, 40, 15};
	SDL_Rect e3 = {centerX + 95, centerY - 50, 35, 15};
	SDL_Rect e4 = {centerX + 95, centerY - 35, 40, 15};
	
	// R
	SDL_Rect r1 = {centerX + 145, centerY - 80, 15, 60};
	SDL_Rect r2 = {centerX + 145, centerY - 80, 35, 15};
	SDL_Rect r3 = {centerX + 145, centerY - 50, 35, 15};
	SDL_Rect r4 = {centerX + 175, centerY - 65, 15, 15};
	SDL_Rect r5 = {centerX + 170, centerY - 35, 15, 25};
	
	SDL_RenderFillRect(renderer, &n1); SDL_RenderFillRect(renderer, &n2); SDL_RenderFillRect(renderer, &n3);
	SDL_RenderFillRect(renderer, &i1);
	SDL_RenderFillRect(renderer, &b1); SDL_RenderFillRect(renderer, &b2); SDL_RenderFillRect(renderer, &b3);
	SDL_RenderFillRect(renderer, &b4); SDL_RenderFillRect(renderer, &b5); SDL_RenderFillRect(renderer, &b6);
	SDL_RenderFillRect(renderer, &b7); SDL_RenderFillRect(renderer, &b8); SDL_RenderFillRect(renderer, &b9);
	SDL_RenderFillRect(renderer, &b10); SDL_RenderFillRect(renderer, &b11); SDL_RenderFillRect(renderer, &b12);
	SDL_RenderFillRect(renderer, &l1); SDL_RenderFillRect(renderer, &l2);
	SDL_RenderFillRect(renderer, &e1); SDL_RenderFillRect(renderer, &e2); SDL_RenderFillRect(renderer, &e3); SDL_RenderFillRect(renderer, &e4);
	SDL_RenderFillRect(renderer, &r1); SDL_RenderFillRect(renderer, &r2); SDL_RenderFillRect(renderer, &r3);
	SDL_RenderFillRect(renderer, &r4); SDL_RenderFillRect(renderer, &r5);
	
	setRenderColor(customWhite);
	
	int textY = centerY + 40;
	for (int i = 0; i < 45; i++) {
		SDL_Rect dot = {centerX - 180 + (i * 8), textY, 5, 5};
		SDL_RenderFillRect(renderer, &dot);
	}
	
	// "1/2/3 to switch libraries"
	textY = centerY + 80;
	for (int i = 0; i < 35; i++) {
		SDL_Rect dot = {centerX - 140 + (i * 8), textY, 5, 5};
		SDL_RenderFillRect(renderer, &dot);
	}
	
	SDL_RenderPresent(renderer);
}

void SDLGraphic::renderGameOver(const GameState& state) {
	(void)state;  // Will be used when we add text rendering for score
	
	setRenderColor(customBlack);
	SDL_RenderClear(renderer);
	
	drawBorder(cellSize);
	
	int windowWidth = (gridWidth * cellSize) + (2 * borderOffset);
	int windowHeight = (gridHeight * cellSize) + (2 * borderOffset);
	int centerX = windowWidth / 2;
	int centerY = windowHeight / 2;
	
	setRenderColor(lightRed);
	
	// G
	SDL_Rect g1 = {centerX - 120, centerY - 60, 15, 60};
	SDL_Rect g2 = {centerX - 120, centerY - 60, 50, 15};
	SDL_Rect g3 = {centerX - 120, centerY - 15, 50, 15};
	SDL_Rect g4 = {centerX - 55, centerY - 40, 15, 25};
	SDL_Rect g5 = {centerX - 85, centerY - 40, 30, 15};
	
	// A
	SDL_Rect a1 = {centerX - 35, centerY - 60, 15, 60};
	SDL_Rect a2 = {centerX - 35, centerY - 60, 40, 15};
	SDL_Rect a3 = {centerX - 35, centerY - 40, 40, 15};
	SDL_Rect a4 = {centerX + 5, centerY - 60, 15, 60};
	
	// M
	SDL_Rect m1 = {centerX + 30, centerY - 60, 15, 60};
	SDL_Rect m2 = {centerX + 30, centerY - 60, 15, 30};
	SDL_Rect m3 = {centerX + 55, centerY - 45, 15, 45};
	SDL_Rect m4 = {centerX + 80, centerY - 60, 15, 30};
	SDL_Rect m5 = {centerX + 80, centerY - 60, 15, 60};
	
	// E (same as before)
	SDL_Rect e1 = {centerX + 105, centerY - 60, 15, 60};
	SDL_Rect e2 = {centerX + 105, centerY - 60, 40, 15};
	SDL_Rect e3 = {centerX + 105, centerY - 40, 35, 15};
	SDL_Rect e4 = {centerX + 105, centerY - 15, 40, 15};
	
	SDL_RenderFillRect(renderer, &g1); SDL_RenderFillRect(renderer, &g2); SDL_RenderFillRect(renderer, &g3);
	SDL_RenderFillRect(renderer, &g4); SDL_RenderFillRect(renderer, &g5);
	SDL_RenderFillRect(renderer, &a1); SDL_RenderFillRect(renderer, &a2); SDL_RenderFillRect(renderer, &a3); SDL_RenderFillRect(renderer, &a4);
	SDL_RenderFillRect(renderer, &m1); SDL_RenderFillRect(renderer, &m2); SDL_RenderFillRect(renderer, &m3);
	SDL_RenderFillRect(renderer, &m4); SDL_RenderFillRect(renderer, &m5);
	SDL_RenderFillRect(renderer, &e1); SDL_RenderFillRect(renderer, &e2); SDL_RenderFillRect(renderer, &e3); SDL_RenderFillRect(renderer, &e4);
	
	// "OVER"
	int overY = centerY + 5;
	
	// O
	SDL_Rect o1 = {centerX - 60, overY, 15, 40};
	SDL_Rect o2 = {centerX - 60, overY, 40, 15};
	SDL_Rect o3 = {centerX - 60, overY + 25, 40, 15};
	SDL_Rect o4 = {centerX - 25, overY, 15, 40};
	
	// V
	SDL_Rect v1 = {centerX, overY, 15, 30};
	SDL_Rect v2 = {centerX + 15, overY + 25, 15, 15};
	SDL_Rect v3 = {centerX + 30, overY, 15, 30};
	
	// E
	SDL_Rect e5 = {centerX + 55, overY, 15, 40};
	SDL_Rect e6 = {centerX + 55, overY, 30, 15};
	SDL_Rect e7 = {centerX + 55, overY + 12, 25, 15};
	SDL_Rect e8 = {centerX + 55, overY + 25, 30, 15};
	
	// R
	SDL_Rect r1 = {centerX + 95, overY, 15, 40};
	SDL_Rect r2 = {centerX + 95, overY, 30, 15};
	SDL_Rect r3 = {centerX + 95, overY + 12, 30, 15};
	SDL_Rect r4 = {centerX + 120, overY, 15, 12};
	SDL_Rect r5 = {centerX + 115, overY + 25, 15, 15};
	
	SDL_RenderFillRect(renderer, &o1); SDL_RenderFillRect(renderer, &o2); SDL_RenderFillRect(renderer, &o3); SDL_RenderFillRect(renderer, &o4);
	SDL_RenderFillRect(renderer, &v1); SDL_RenderFillRect(renderer, &v2); SDL_RenderFillRect(renderer, &v3);
	SDL_RenderFillRect(renderer, &e5); SDL_RenderFillRect(renderer, &e6); SDL_RenderFillRect(renderer, &e7); SDL_RenderFillRect(renderer, &e8);
	SDL_RenderFillRect(renderer, &r1); SDL_RenderFillRect(renderer, &r2); SDL_RenderFillRect(renderer, &r3);
	SDL_RenderFillRect(renderer, &r4); SDL_RenderFillRect(renderer, &r5);
	
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