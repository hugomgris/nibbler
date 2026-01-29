#include "../../incs/ParticleSystem.hpp"
#include <cmath>
#include <algorithm>
#include <cstdlib>

// Particle constructors
Particle::Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime)
	: x(px), y(py), vx(0), vy(0), age(0.0f), type(ParticleType::Dust), color({ 255, 248, 227, 255}) {
	initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
	currentSize = initialSize;
	lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
	rotation = static_cast<float>(rand() % 360);
	rotationSpeed = -30.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 60.0f;  // -30 to +30 deg/s
}

Particle::Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime, 
		float velocityX, float velocityY, SDL_Color particleColor)
	: x(px), y(py), vx(velocityX), vy(velocityY), age(0.0f), type(ParticleType::Explosion), color(particleColor) {
	initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
	currentSize = initialSize;
	lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
	rotation = static_cast<float>(rand() % 360);
	rotationSpeed = -50.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;  // -50 to +50 deg/s for explosions
}

// ParticleSystem implementation
ParticleSystem::ParticleSystem(SDL_Renderer* renderer, int gridW, int gridH, int cell, int border)
	: renderer(renderer), gridWidth(gridW), gridHeight(gridH), cellSize(cell), borderOffset(border),
		maxDustDensity(50), dustSpawnInterval(0.1f), dustSpawnTimer(0.0f),
		dustMinSize(2.0f), dustMaxSize(15.0f), dustMinLifetime(3.0f), dustMaxLifetime(5.0f),
		explosionMinSize(1.0f), explosionMaxSize(50.0f) {
	particles.reserve(maxDustDensity);
}

ParticleSystem::~ParticleSystem() {
	particles.clear();
}

void ParticleSystem::update(float deltaTime) {
	// Handle dust particle spawning
	dustSpawnTimer += deltaTime;
	if (dustSpawnTimer >= dustSpawnInterval) {
		spawnDustParticle();
		dustSpawnTimer = 0.0f;
	}
	
	// Update all particles
	for (auto& particle : particles) {
		particle.age += deltaTime;
		particle.rotation += particle.rotationSpeed * deltaTime;
		
		// Movement (for explosions and trails)
		particle.x += particle.vx * deltaTime;
		particle.y += particle.vy * deltaTime;
		
		// Shrinking effect
		float progress = particle.age / particle.lifetime;
		particle.currentSize = particle.initialSize * (1.0f - progress) + 1.0f * progress;
	}
	
	// Remove dead particles
	particles.erase(
		std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.age >= p.lifetime; }),
		particles.end()
	);
}

void ParticleSystem::render() {
	for (const auto& particle : particles) {
		// Fade out based on lifetime progress
		float progress = particle.age / particle.lifetime;
		Uint8 alpha;
		
		// Alpha handling based on particle type
		if (particle.type == ParticleType::Dust) {
			alpha = static_cast<Uint8>((1.0f - progress) * 120);
		} else {  // Explosion or Trail
			alpha = static_cast<Uint8>((1.0f - progress) * 200);
		}
		
		drawRotatedSquare(particle.x, particle.y, particle.currentSize, 
		                  particle.rotation, particle.color, alpha);
	}
}

void ParticleSystem::spawnDustParticle() {
	// max density check
	int dustCount = 0;
	for (const auto& p : particles) {
		if (p.type == ParticleType::Dust) dustCount++;
	}
	if (dustCount >= maxDustDensity) return;
	
	// Control the spawning area -> i.e., spawn inside the border boundaries
	int arenaX = borderOffset;
	int arenaY = borderOffset;
	int arenaW = gridWidth * cellSize;
	int arenaH = gridHeight * cellSize;
	
	float x = arenaX + static_cast<float>(rand() % arenaW);
	float y = arenaY + static_cast<float>(rand() % arenaH);
	
	particles.emplace_back(x, y, dustMinSize, dustMaxSize, dustMinLifetime, dustMaxLifetime);
}

void ParticleSystem::spawnExplosion(float x, float y, int count) {
	for (int i = 0; i < count; i++) {

		float angle = (rand() % 360) * 3.14159f / 180.0f;
		float speed = 50.0f + (rand() % 150);	// 50-200 pixels/sec
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		SDL_Color explosionColor = {254, 74, 81, 255};	// lightRed
		
		particles.emplace_back(x, y, explosionMinSize, explosionMaxSize, 0.5f, 1.0f, vx, vy, explosionColor);
	}
}

void ParticleSystem::spawnDirectedParticles(float x, float y, int count, float direction, 
											float spread, float minSpeed, float maxSpeed) {
	float baseAngle = direction * 3.14159f / 180.0f;
	float spreadRad = spread * 3.14159f / 180.0f;
	
	for (int i = 0; i < count; i++) {
		// Random angle within spread cone
		float angleOffset = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * spreadRad;
		float angle = baseAngle + angleOffset;
		
		float speed = minSpeed + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxSpeed - minSpeed);
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		SDL_Color color = {70, 130, 180, 255};	// lightBlue
		
		particles.emplace_back(x, y, 5.0f, 15.0f, 1.0f, 2.0f, vx, vy, color);
	}
}

void ParticleSystem::spawnDirectedParticlesInArea(float centerX, float centerY, float areaWidth, float areaHeight,
													int count, float direction, float spread,
													float minSpeed, float maxSpeed, SDL_Color color) {
	float baseAngle = direction * 3.14159f / 180.0f;
	float spreadRad = spread * 3.14159f / 180.0f;
	
	for (int i = 0; i < count; i++) {
		// Random spawn position within area
		float spawnX = centerX + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * areaWidth;
		float spawnY = centerY + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * areaHeight;
		
		// Random angle within spread cone
		float angleOffset = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * spreadRad;
		float angle = baseAngle + angleOffset;
		
		// Random speed
		float speed = minSpeed + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxSpeed - minSpeed);
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		particles.emplace_back(spawnX, spawnY, 5.0f, 15.0f, 1.0f, 2.0f, vx, vy, color);
	}
}

void ParticleSystem::spawnSnakeTrail(float x, float y, int count, float direction, SDL_Color color) {
	float angle = direction * 3.14159f / 180.0f;
	
	for (int i = 0; i < count; i++) {
		float offsetX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 30.0f;
		float offsetY = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 30.0f;
		float spawnX = x + offsetX;
		float spawnY = y + offsetY;
		
		float speed = 30.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 9.0f;
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		float minSize = 20.0f;
		float maxSize = 25.0f;
		float lifetime = 0.2f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 1.5f;
		
		// here particles have no rotation because I want a straight trail
		Particle particle(spawnX, spawnY, minSize, maxSize, lifetime, lifetime, vx, vy, color);
		particle.rotation = 0.0f;
		particle.rotationSpeed = 0.0f;
		particle.type = ParticleType::Trail;
		particles.push_back(particle);
	}
}

void ParticleSystem::drawRotatedSquare(float cx, float cy, float size, float rotation, SDL_Color color, Uint8 alpha) {
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
