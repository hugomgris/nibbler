#pragma once
#include <raylib.h>
#include <vector>
#include <cmath>
#include <algorithm>

enum class ParticleType {
	Dust,
	Explosion,
	Trail
};

struct Particle {
	float			x, y;				// Center position
	float			vx, vy;				// Velocity for movement
	float			rotation;          
	float			rotationSpeed;     
	float			initialSize;
	float			currentSize;
	float			lifetime;
	float			age;
	ParticleType	type;
	Color			color;
	
	// Constructor for Dust particles
	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime);
	
	// Constructor for Explosion and Trail particles
	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime, 
			float velocityX, float velocityY, Color particleColor);
};

class ParticleSystem {
	private:
		std::vector<Particle>	particles;
		
		// Grid dimensions for boundary checking
		int		gridWidth;
		int		gridHeight;
		int		cellSize;
		int		borderOffset;
		
		// Dust particle settings
		int		maxDustDensity;
		float	dustSpawnInterval;
		float	dustSpawnTimer;
		float	dustMinSize;
		float	dustMaxSize;
		float	dustMinLifetime;
		float	dustMaxLifetime;
		
		// Explosion particle settings
		float	explosionMinSize;
		float	explosionMaxSize;
		
		// Helper to draw a rotated square particle
		void drawRotatedSquare(float cx, float cy, float size, float rotation, Color color, unsigned char alpha);

	public:
		ParticleSystem ();
		ParticleSystem (int gridW, int gridH, int cell, int border, int maxDust = 30, float dustInterval = 0.15f);
		ParticleSystem (const ParticleSystem &other) = delete;
		ParticleSystem &operator=(const ParticleSystem &other) = delete;

		~ParticleSystem ();
		
		// Update and render
		void update(float deltaTime);
		void render();
		
		// Spawning functions
		void spawnDustParticle();
		void spawnExplosion(float x, float y, int count);
		void spawnDirectedParticles(float x, float y, int count, float direction, float spread, 
									float minSpeed = 50.0f, float maxSpeed = 200.0f);
		void spawnDirectedParticlesInArea(float centerX, float centerY, float areaWidth, float areaHeight, 
										int count, float direction, float spread, 
										float minSpeed, float maxSpeed, Color color);
		void spawnSnakeTrail(float x, float y, int count, float direction, Color color);
		
		// Configuration
		void setMaxDustDensity(int density);
		void setDustSpawnInterval(float interval);
		
		// Utility
		void clear();
		size_t getParticleCount() const;
};
