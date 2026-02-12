#include "../../incs/ParticleSystem.hpp"
#include <raymath.h>
#include <rlgl.h>
#include <cstdlib>
#include <iostream>

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
		float velocityX, float velocityY, Color particleColor)
	: x(px), y(py), vx(velocityX), vy(velocityY), age(0.0f), type(ParticleType::Explosion), color(particleColor) {
	initialSize = minSize + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxSize - minSize);
	currentSize = initialSize;
	lifetime = minLifetime + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxLifetime - minLifetime);
	rotation = static_cast<float>(rand() % 360);
	rotationSpeed = -50.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;  // -50 to +50 deg/s for explosions
}

// system implementation
ParticleSystem::ParticleSystem(int gridW, int gridH, int cell, int border, int maxDust, float dustInterval)
	: gridWidth(gridW), gridHeight(gridH), cellSize(cell), borderOffset(border),
		maxDustDensity(maxDust), dustSpawnInterval(dustInterval), dustSpawnTimer(0.0f),
		dustMinSize(4.0f), dustMaxSize(20.0f), dustMinLifetime(3.0f), dustMaxLifetime(5.0f),  // Increased sizes
		explosionMinSize(1.0f), explosionMaxSize(50.0f) {
	particles.reserve(maxDustDensity);
}

ParticleSystem::~ParticleSystem() {
	particles.clear();
}

void ParticleSystem::update(float deltaTime) {
	// dust particle spawning
	dustSpawnTimer += deltaTime;
	if (dustSpawnTimer >= dustSpawnInterval) {
		spawnDustParticle();
		dustSpawnTimer = 0.0f;
	}
	
	// update ALL particles
	for (auto& particle : particles) {
		particle.age += deltaTime;
		particle.rotation += particle.rotationSpeed * deltaTime;
		
		// Movement (for explosions and trails)
		particle.x += particle.vx * deltaTime;
		particle.y += particle.vy * deltaTime;
		
		// shrinking 
		float progress = particle.age / particle.lifetime;
		particle.currentSize = particle.initialSize * (1.0f - progress) + 1.0f * progress;
	}
	
	// remove dead particles
	particles.erase(
		std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.age >= p.lifetime; }),
		particles.end()
	);
}

void ParticleSystem::render() {
	static int renderCount = 0;
	if (renderCount < 3 && particles.size() > 0) {
		std::cout << "Render #" << renderCount << ": " << particles.size() << " particles" << std::endl;
		if (particles.size() > 0) {
			const auto& p = particles[0];
			float progress = p.age / p.lifetime;
			unsigned char alpha = static_cast<unsigned char>((1.0f - progress) * 200);
			std::cout << "  First particle: pos=(" << p.x << "," << p.y << ") size=" << p.currentSize 
			          << " alpha=" << (int)alpha << " age=" << p.age << "/" << p.lifetime << std::endl;
		}
		renderCount++;
	}
	
	for (const auto& particle : particles) {
		// Fade out (lifetime progress)
		float progress = particle.age / particle.lifetime;
		unsigned char alpha;
		
		// alpha handling 
		if (particle.type == ParticleType::Dust) {
			alpha = static_cast<unsigned char>((1.0f - progress) * 200);  // Increased from 120
		} else {  // explosion or Trail
			alpha = static_cast<unsigned char>((1.0f - progress) * 255);  // Increased from 200
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
	
	// Spawn across entire screen for menu effects
	// gridWidth/gridHeight are now in screen units (1920x1080)
	int arenaX = 0;
	int arenaY = 0;
	int arenaW = gridWidth;
	int arenaH = gridHeight;
	
	float x = arenaX + static_cast<float>(rand() % arenaW);
	float y = arenaY + static_cast<float>(rand() % arenaH);
	
	particles.emplace_back(x, y, dustMinSize, dustMaxSize, dustMinLifetime, dustMaxLifetime);
	
	// DEBUG - Log first few particles to verify spawning
	/* static int spawnCount = 0;
	if (spawnCount < 5) {
		std::cout << "Spawned dust particle #" << spawnCount << " at (" << x << ", " << y << ") in area " << arenaW << "x" << arenaH << std::endl;
		spawnCount++;
	} */
}

void ParticleSystem::spawnExplosion(float x, float y, int count) {
	for (int i = 0; i < count; i++) {
		float angle = (rand() % 360) * DEG2RAD;
		float speed = 50.0f + (rand() % 150);	// 50-200 pixels/sec
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		Color explosionColor = {254, 74, 81, 255};	// lightRed
		
		particles.emplace_back(x, y, explosionMinSize, explosionMaxSize, 0.5f, 1.0f, vx, vy, explosionColor);
	}
}

void ParticleSystem::spawnDirectedParticles(float x, float y, int count, float direction, 
											float spread, float minSpeed, float maxSpeed) {
	float baseAngle = direction * DEG2RAD;
	float spreadRad = spread * DEG2RAD;
	
	for (int i = 0; i < count; i++) {
		// Random angle within spread cone
		float angleOffset = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * spreadRad;
		float angle = baseAngle + angleOffset;
		
		float speed = minSpeed + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxSpeed - minSpeed);
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		Color color = {70, 130, 180, 255};	// lightBlue
		
		particles.emplace_back(x, y, 5.0f, 15.0f, 1.0f, 2.0f, vx, vy, color);
	}
}

void ParticleSystem::spawnDirectedParticlesInArea(float centerX, float centerY, float areaWidth, float areaHeight,
													int count, float direction, float spread,
													float minSpeed, float maxSpeed, Color color) {
	float baseAngle = direction * DEG2RAD;
	float spreadRad = spread * DEG2RAD;
	
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

void ParticleSystem::spawnSnakeTrail(float x, float y, int count, float direction, Color color) {
	float angle = direction * DEG2RAD;
	
	for (int i = 0; i < count; i++) {
		float offsetX = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 30.0f;
		float offsetY = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 30.0f;
		float spawnX = x + offsetX;
		float spawnY = y + offsetY;
		
		float speed = 50.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 9.0f;
		float vx = cosf(angle) * speed;
		float vy = sinf(angle) * speed;
		
		/* float minSize = (x <= 1135.0f) ? 10.0f : 20.0f;
		float maxSize = (x <= 1135.0f) ? 15.0f : 25.0f; */
		float minSize = 10.0f;
		float maxSize = 20.0f;
		float lifetime = 0.2f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 1.5f;
		
		// here particles have no rotation because I want a straight trail
		Particle particle(spawnX, spawnY, minSize, maxSize, lifetime, lifetime, vx, vy, color);
		particle.rotation = 0.0f;
		particle.rotationSpeed = 0.0f;
		particle.type = ParticleType::Trail;
		particles.push_back(particle);
	}
}

void ParticleSystem::drawRotatedSquare(float cx, float cy, float size, float rotation, Color color, unsigned char alpha) {
	// Mainly for dust particles
	Color colorWithAlpha = {color.r, color.g, color.b, alpha};
	
	Rectangle rect = {cx, cy, size, size};
	Vector2 origin = {size / 2.0f, size / 2.0f};
	
	DrawRectanglePro(rect, origin, rotation, colorWithAlpha);
}

// Configuration
void ParticleSystem::setMaxDustDensity(int density) { maxDustDensity = density; }
void ParticleSystem::setDustSpawnInterval(float interval) { dustSpawnInterval = interval; }
		
// Utility
void ParticleSystem::clear() { particles.clear(); }
size_t ParticleSystem::getParticleCount() const { return particles.size(); }
