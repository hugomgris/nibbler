# Nibbler - Devlog - 8

## Table of Contents
1. [Day Eight Plan](#81-day-eight-plan)
2. [Some More Thinking on the Walk](#82-some-more-thinking-on-the-walk)
3. [More, More, More!!! (particles)](#83-more-more-more-particles)

<br>
<br>
<br>

## 8.1 Day Eight Plan
Today the 2D realm will be done task-wise. For it, the following needs to happen:
- Particles spray when eating
- Some animation of the food
	- Maybe regarding the spawning (tunneling?, which would mean I'd have to animate the spawning in 3D too, though)
- Think about if the snake is going to be a solid stripe of plain color, or if some effects are to be put in place
	- I'd like to add some print-like texturing, but that might be hard (or too much of a hustle regarding the taks turn in)

Some other things that I'd like to tackle today:
- Make sure that the 3D rendering in Raylib is always completely inside and centered in the game window (specially check big arenas)
- See if the `Ncurses` rendering can be done in a floating window/terminal, i.e. dettaching it from the main console from which the game is ran
- Implement a pause button (spacebar)

<br>
<br>
<br>

## 8.2 Some More Thinking on the Walk
This morning, while commuting and having my morning walk, I thought that **refocusing the mental ideal of what this game could be around the notion of `beads` could be a good idea**. From *Nido* to *Rosario*, building a game based on snake body modules (and heads) with effects/abilities/etc. Trying to build *from* there, starting in a pure gameplay oriented prototype and see what it could lead to. 
 > all of this while having a hundred other projects in my head, all fighting for some attention. Well, at least the idea is there...

<br>
<br>
<br>

 ## 8.3 More, More, More!!! (Particles)
 Yesterday I built a particle pipeline focused on dust particles for atmosphere, but if I want to expand the particle landscape to contain more types of particles, the system should be refactored into a general particle managing pipeline. This entrails a handful of changes, so let's get to it:
 
 First, let's rebuild the particle data with a new enum for `ParticleType`, and expand the `Particle` class (renamed from `DustParticle` so that it now holds attributes for `velocity`, `color` and `type`):
 ```cpp
 enum class ParticleType {
    Dust,
    Explosion
};
```

> This new enum is a `class` for pollution reasons: I want it to be strongly typed and scoped, as well as to avoid enumeration conflicts. (It might not be strictly needed in this case, but for the sake of trying new things =D)

```cpp
struct Particle {
	float			x, y;				// Center position
	float			vx, vy;				// Neded for explosion, maybe something else/more in the future
	float			rotation;          
	float			rotationSpeed;     
	float			initialSize;
	float			currentSize;
	float			lifetime;
	float			age;
	ParticleType	type;
	SDL_Color		color;
	
	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime)
		: x(px), y(py), vx(0), vy(0), age(0.0f), type(ParticleType::Dust), color({ 255, 248, 227, 255}) {
		// constructor logic
	}

	Particle(float px, float py, float minSize, float maxSize, float minLifetime, float maxLifetime, 
			float velocityX, float velocityY, SDL_Color particleColor)
		: x(px), y(py), vx(velocityX), vy(velocityY), age(0.0f), type(ParticleType::Explosion), color(particleColor) {
		// constructor logic
	}
};
```
> Each type of `Particle` has its own logic, but it is just comprised of manually tweaked numbers. Also, I'm building this as a Struct because I want to keep the prototype's graphics files limited to one for each implemented library. A more sane approach might be to have a particle class, but maybe no, I don't know, so my best move is: whatever.

From here, the whole Particle System is repurposed:
- `update()` and `render()` will manage every particle across types
- `spawn()` will be split into type-specific functions
- Everything will still be based on rotating squares (I don't want to deal with drawing any other type of primitive, it's not worth it in this prototype)

```cpp
void updateParticles(float deltaTime);
void renderParticles();
void spawnDustParticle();
void spawnExplosion(float x, float y, int count);
void drawRotatedSquare(float cx, float cy, float size, float rotation, SDL_Color color, Uint8 alpha);
```
```cpp
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
```
```cpp
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
```
```cpp
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
```
```cpp
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
```
```cpp
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
```
And thus the `SDL2`, 2D realm of the prototype is visually done in regards to the task. If I find myself with spare time, I might tweak things here and there, but for now I'll move to some other stuff.


