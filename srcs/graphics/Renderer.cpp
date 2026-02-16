#include "../../incs/Renderer.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/RaylibColors.hpp"
#include "../../incs/ParticleSystem.hpp"
#include "../../incs/TextSystem.hpp"
#include "../../incs/AnimationSystem.hpp"
#include "../../incs/MenuSystem.hpp"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

Renderer::Renderer() :
	screenWidth(1920),
	screenHeight(1080),
	cubeSize(2.0f),
	cameraSize(0.0f),
	menuFov(50.0f),
	customFov(0.0f),
	squareSize(50),
	separator(0.0f),
	gridWidth(0),
	gridHeight(0),
	borderThickness(25),
	arenaWidth(0),
	arenaHeight(0),
	arenaOffsetX(0),
	arenaOffsetY(0),
	gameAreaX(0),
	gameAreaY(0),
	accumulatedTime(0.0f) {
		separator = cubeSize * 2;
	}

Renderer::~Renderer() {
	CloseWindow();
	std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
}

void Renderer::init(int width, int height) {
	gridWidth = width;
	gridHeight = height;
	
	// IMPORTANT: Set VSync flag BEFORE InitWindow
	SetConfigFlags(FLAG_VSYNC_HINT);
	
	InitWindow(screenWidth, screenHeight, "Nibbler 3D - Raylib");
	ToggleFullscreen();
	SetTargetFPS(60);  // Lock to 60 FPS
	
	setupCamera3D();
	
	// Setup 2D camera for UI rendering
	camera2D.offset = (Vector2){ 0.0f, 0.0f };
	camera2D.target = (Vector2){ 0.0f, 0.0f };
	camera2D.rotation = 0.0f;
	camera2D.zoom = 1.0f;
	
	// Calculate 2D layout (arena centering)
	calculate2DLayout();
	
	std::cout << BYEL << "[Raylib] Initialized: " << width << "x" << height << RESET << std::endl;
	std::cout << BYEL << "[2D Layout] Arena: " << arenaWidth << "x" << arenaHeight 
	          << " at (" << arenaOffsetX << ", " << arenaOffsetY << ")" << RESET << std::endl;
}

// -----3D PIPELINE----- //

void Renderer::render3D(const GameState& state, float deltaTime){
	camera3D.fovy = customFov;
	
	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }

	drawGroundPlane3D();
	drawSnake3D(state.snake_A, snakeAHidden, 
							snakeALightFront, snakeALightTop, snakeALightSide,
							snakeADarkFront, snakeADarkTop, snakeADarkSide);
	
	if (state.config.mode == GameMode::MULTI) {
		drawSnake3D(state.snake_B, snakeBHidden,
			snakeBLightFront, snakeBLightTop, snakeBLightSide,
			snakeBDarkFront, snakeBDarkTop, snakeBDarkSide);
	} else if (state.config.mode == GameMode::AI) {
		drawSnake3D(state.snake_B, snakeAIHidden,
			snakeAILightFront, snakeAILightTop, snakeAILightSide,
			snakeAIDarkFront, snakeAIDarkTop, snakeAIDarkSide);
	}

	drawFood3D(state.food);
}

void Renderer::drawCubeCustomFaces(Vector3 position, float width, float height, float length,
                                         Color front, Color back, Color top, Color bottom, Color right, Color left) {
	float x = position.x;
	float y = position.y;
	float z = position.z;
	
	// In isometric view, typically visible faces are: front (+Z), top (+Y), right (+X)
	rlPushMatrix();
	rlTranslatef(x, y, z);
	
	rlBegin(RL_QUADS);
	
	// Front face (+Z) - typically visible in isometric
	rlColor4ub(front.r, front.g, front.b, front.a);
	rlVertex3f(-width/2, -height/2, length/2);
	rlVertex3f(width/2, -height/2, length/2);
	rlVertex3f(width/2, height/2, length/2);
	rlVertex3f(-width/2, height/2, length/2);
	
	// Back face (-Z)
	rlColor4ub(back.r, back.g, back.b, back.a);
	rlVertex3f(-width/2, -height/2, -length/2);
	rlVertex3f(-width/2, height/2, -length/2);
	rlVertex3f(width/2, height/2, -length/2);
	rlVertex3f(width/2, -height/2, -length/2);
	
	// Top face (+Y) - typically visible in isometric
	rlColor4ub(top.r, top.g, top.b, top.a);
	rlVertex3f(-width/2, height/2, -length/2);
	rlVertex3f(-width/2, height/2, length/2);
	rlVertex3f(width/2, height/2, length/2);
	rlVertex3f(width/2, height/2, -length/2);
	
	// Bottom face (-Y)
	rlColor4ub(bottom.r, bottom.g, bottom.b, bottom.a);
	rlVertex3f(-width/2, -height/2, -length/2);
	rlVertex3f(width/2, -height/2, -length/2);
	rlVertex3f(width/2, -height/2, length/2);
	rlVertex3f(-width/2, -height/2, length/2);
	
	// Right face (+X) - typically visible in isometric
	rlColor4ub(right.r, right.g, right.b, right.a);
	rlVertex3f(width/2, -height/2, -length/2);
	rlVertex3f(width/2, height/2, -length/2);
	rlVertex3f(width/2, height/2, length/2);
	rlVertex3f(width/2, -height/2, length/2);
	
	// Left face (-X)
	rlColor4ub(left.r, left.g, left.b, left.a);
	rlVertex3f(-width/2, -height/2, -length/2);
	rlVertex3f(-width/2, -height/2, length/2);
	rlVertex3f(-width/2, height/2, length/2);
	rlVertex3f(-width/2, height/2, -length/2);
	
	rlEnd();
	rlPopMatrix();
}

void Renderer::setupCamera3D() {
	// Grid is now centered at origin (0, 0, 0)
	float centerX = 0.0f;
	float centerZ = 0.0f;
	
	float diagonal = sqrtf(gridWidth * gridWidth + gridHeight * gridHeight) * cubeSize;
	float distance = diagonal * 2.2f;  // 20% padding
	
	float elevation = 35.264f * DEG2RAD;  // Classic isometric angle
	float rotation = 45.0f * DEG2RAD;
	
	camera3D.position = (Vector3){ 
		centerX + distance * cosf(rotation) * cosf(elevation),
		distance * sinf(elevation),
		centerZ + distance * sinf(rotation) * cosf(elevation)
	};
	
	camera3D.target = (Vector3){ centerX, 0.0f, centerZ };
	camera3D.up = (Vector3){ 0.0f, 1.0f, 0.0f };

	cameraSize = static_cast<float>((gridWidth + gridHeight) / 2);
	customFov = 0.022619f * cameraSize * cameraSize + 0.198810f * cameraSize + 31.028571f;
	
	camera3D.fovy = customFov;
	camera3D.projection = CAMERA_ORTHOGRAPHIC;
}

void Renderer::drawGroundPlane3D() {
	float offsetX = (gridWidth * cubeSize) / 2.0f;
	float offsetZ = (gridHeight * cubeSize) / 2.0f;
	
	for (int z = 0; z < gridHeight; z++) {
		for (int x = 0; x < gridWidth; x++) {		
			Vector3 position = {
				x * cubeSize - offsetX,
				0.0f,
				z * cubeSize - offsetZ
			};
			
			if ((x + z) % 2 == 0) {
				drawCubeCustomFaces(position, cubeSize, cubeSize, cubeSize,
									groundLightFront, groundHidden, groundLightTop, groundHidden, groundLightSide, groundHidden);
			}
			else {
				drawCubeCustomFaces(position, cubeSize, cubeSize, cubeSize,
									groundDarkFront, groundHidden, groundDarkTop, groundHidden, groundDarkSide, groundHidden);
			}
		}
	}
}

// not used right now, but not sure if deprecated yet either, so keeping it here for now
void Renderer::drawWalls3D() {
	for (int level = 0; level < 3; level++) {
		float yPos = (level) * cubeSize;
		
		for (int x = -1; x <= gridWidth; x++) {
			// Top wall
			Vector3 topPos = { x * cubeSize, yPos, -cubeSize };
			DrawCube(topPos, cubeSize, cubeSize, cubeSize * 2, wallColor);
			
			// Bottom wall
			Vector3 bottomPos = { x * cubeSize, yPos, gridHeight * cubeSize };
			DrawCube(bottomPos, cubeSize, cubeSize, cubeSize, wallColorFade);
		}
		
		for (int z = 0; z < gridHeight; z++) {
			// Left wall
			Vector3 leftPos = { -cubeSize, yPos, z * cubeSize };
			DrawCube(leftPos, cubeSize, cubeSize, cubeSize, wallColor);
			
			// Right wall
			Vector3 rightPos = { gridWidth * cubeSize, yPos, z * cubeSize };
			DrawCube(rightPos, cubeSize, cubeSize, cubeSize, wallColor);
		}
	}
}

void Renderer::drawSnake3D(const Snake* snake, Color hidden,
	Color lightFront,  Color lightTop, Color lightSide,
	Color darkFront, Color darkTop, Color darkSide) {
	float yPos = cubeSize;
	
	// Calculate offset to match grid centering
	float offsetX = (gridWidth * cubeSize) / 2.0f;
	float offsetZ = (gridHeight * cubeSize) / 2.0f;
	
	for (int i = 0; i < snake->getLength(); i++) {
		const Vec2& segment = snake->getSegments()[i];
		
		Vector3 position = {
			segment.x * cubeSize - offsetX,
			yPos,
			segment.y * cubeSize - offsetZ
		};
		
		// Head is full size, body is 80% size
		float size = (i == 0) ? cubeSize : cubeSize * 0.8f;
		if (i > 0) position.y *= 0.8f;  // Adjust Y position for body
		
		// Checkerboard pattern for all segments
		if (i % 2 == 0) {
			drawCubeCustomFaces(position, size, size, size,
								lightFront, hidden, lightTop, hidden, lightSide, hidden);
		} else {
			drawCubeCustomFaces(position, size, size, size,
								darkFront, hidden, darkTop, hidden, darkSide, hidden);
		}
	}
}

void Renderer::drawFood3D(const Food* food) {
	float yPos = cubeSize;
	
	// Calculate offset to match grid centering
	float offsetX = (gridWidth * cubeSize) / 2.0f;
	float offsetZ = (gridHeight * cubeSize) / 2.0f;
	
	Vec2 foodPos = food->getPosition();
	Vector3 position = {
		foodPos.x * cubeSize - offsetX,
		yPos,
		foodPos.y * cubeSize - offsetZ
	};
	
	// Pulsing effect
	float pulse = 1.0f + sinf(accumulatedTime * 3.0f) * 0.1f;

	drawCubeCustomFaces(position, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse,
						foodFront, foodHidden, foodTop, foodHidden, foodSide, foodHidden);
}


// -----2D PIPELINE----- //

void Renderer::calculate2DLayout() {
	// Calculate the total arena size (grid + borders)
	arenaWidth = (gridWidth * squareSize) + (2 * borderThickness);
	arenaHeight = (gridHeight * squareSize) + (2 * borderThickness);
	
	// Center the arena in the screen
	arenaOffsetX = (screenWidth - arenaWidth) / 2.0f;
	arenaOffsetY = (screenHeight - arenaHeight) / 2.0f;
	
	// Calculate where the actual game grid starts (after the border)
	gameAreaX = arenaOffsetX + borderThickness;
	gameAreaY = arenaOffsetY + borderThickness;
}

Vector2 Renderer::gridToScreen2D(int gridX, int gridY) const {
	return {
		gameAreaX + (gridX * squareSize),
		gameAreaY + (gridY * squareSize)
	};
}

void Renderer::render2D(const GameState& state, float deltaTime, ParticleSystem& particles, AnimationSystem& animations, Color color){
	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }

	// Render background effects
	particles.render();
	
	// Render tunnel effect with custom centered border
	animations.renderTunnelEffectCustom(
		static_cast<int>(arenaOffsetX),
		static_cast<int>(arenaOffsetY),
		static_cast<int>(arenaOffsetX + arenaWidth),
		static_cast<int>(arenaOffsetY + arenaHeight)
	);

	// Draw snakes
	drawSnake2D(state.snake_A, color, particles, snakeA_tailState);
	
	if (state.config.mode == GameMode::MULTI) {
		drawSnake2D(state.snake_B, snakeBLightFront, particles, snakeB_tailState);
	} else if (state.config.mode == GameMode::AI)
		drawSnake2D(state.snake_B, snakeAILightSide, particles, snakeB_tailState);

	// Draw food
	drawFood2D(state.food, particles);

	// Draw centered border for 2D game
	drawBorderCentered(borderThickness);
}

void Renderer::drawSnake2D(const Snake* snake, Color color, ParticleSystem& particles, TailState& tailState) {
	const Vec2* segments = snake->getSegments();
	int length = snake->getLength();
	
	// Draw snake body segments
	for (int i = 0; i < length; ++i) {
		Vector2 screenPos = gridToScreen2D(segments[i].x, segments[i].y);
		
		// Draw the square segment
		DrawRectangle(
			static_cast<int>(screenPos.x),
			static_cast<int>(screenPos.y),
			squareSize,
			squareSize,
			color
		);
		
		// Add particle trail for the tail segment
		if (i == length - 1 && length > 1) {
			float tailX = screenPos.x + (squareSize / 2.0f);
			float tailY = screenPos.y + (squareSize / 2.0f);
			
			if (!tailState.isFirstFrame) {
				// Calculate distance from last position
				float dx = tailX - tailState.lastX;
				float dy = tailY - tailState.lastY;
				float distance = sqrtf(dx * dx + dy * dy);
				
				// Calculate direction for trail
				Vec2 tail = segments[i];
				Vec2 beforeTail = segments[i - 1];
				float direction = 0.0f;
				
				if (tail.x > beforeTail.x) direction = 0.0f;          // Moving right
				else if (tail.x < beforeTail.x) direction = 180.0f;   // Moving left
				else if (tail.y > beforeTail.y) direction = 90.0f;    // Moving down
				else if (tail.y < beforeTail.y) direction = 270.0f;   // Moving up
				
				// Spawn particles along the path if the tail moved
				if (distance > 1.0f) {
					int steps = static_cast<int>(distance / 15.0f) + 1;
					
					for (int step = 0; step < steps; ++step) {
						float t = static_cast<float>(step) / static_cast<float>(steps);
						float interpX = tailState.lastX + (dx * t);
						float interpY = tailState.lastY + (dy * t);
						
						particles.spawnSnakeTrail(interpX - 10.0f, interpY - 10.0f, 1, direction, color);
					}
				}
			}
			
			// Update tail state
			tailState.lastX = tailX;
			tailState.lastY = tailY;
			tailState.isFirstFrame = false;
		}
	}
}

void Renderer::drawFood2D(const Food* food, ParticleSystem& particles) {
	Vec2 foodPos = food->getPosition();
	Vector2 screenPos = gridToScreen2D(foodPos.x, foodPos.y);
	
	// Check if food position changed and spawn explosion at OLD position
	if (lastFoodX != -1 && (lastFoodX != foodPos.x || lastFoodY != foodPos.y)) {
		// Convert OLD food position to screen coordinates
		Vector2 oldScreenPos = gridToScreen2D(lastFoodX, lastFoodY);
		float explosionX = oldScreenPos.x + (squareSize / 2.0f);
		float explosionY = oldScreenPos.y + (squareSize / 2.0f);
		
		// Spawn explosion particles at the OLD position (where food was eaten)
		particles.spawnExplosion(explosionX, explosionY, 20);
	}
	
	// Update last food position AFTER checking for explosion
	lastFoodX = foodPos.x;
	lastFoodY = foodPos.y;
	
	// Draw the food square with pulsing effect at NEW position
	float pulse = 1.0f + sinf(accumulatedTime * 3.0f) * 0.1f;
	int pulseSize = static_cast<int>(squareSize * 0.7f * pulse);
	int offset = (squareSize - pulseSize) / 2;
	
	DrawRectangle(
		static_cast<int>(screenPos.x) + offset,
		static_cast<int>(screenPos.y) + offset,
		pulseSize,
		pulseSize,
		foodFront
	);
}

void Renderer::drawBorderCentered(int thickness) {
	// Top border
	DrawRectangle(
		static_cast<int>(arenaOffsetX),
		static_cast<int>(arenaOffsetY),
		static_cast<int>(arenaWidth),
		thickness,
		customWhite
	);
	
	// Bottom border
	DrawRectangle(
		static_cast<int>(arenaOffsetX),
		static_cast<int>(arenaOffsetY + arenaHeight - thickness),
		static_cast<int>(arenaWidth),
		thickness,
		customWhite
	);
	
	// Left border
	DrawRectangle(
		static_cast<int>(arenaOffsetX),
		static_cast<int>(arenaOffsetY),
		thickness,
		static_cast<int>(arenaHeight),
		customWhite
	);
	
	// Right border
	DrawRectangle(
		static_cast<int>(arenaOffsetX + arenaWidth - thickness),
		static_cast<int>(arenaOffsetY),
		thickness,
		static_cast<int>(arenaHeight),
		customWhite
	);
}

void Renderer::drawBorderFullscreen(int thickness) {
	// Top border
	DrawRectangle(0, 0, screenWidth, thickness, customWhite);
	
	// Bottom border
	DrawRectangle(0, screenHeight - thickness, screenWidth, thickness, customWhite);
	
	// Left border
	DrawRectangle(0, 0, thickness, screenHeight, customWhite);
	
	// Right border
	DrawRectangle(screenWidth - thickness, 0, thickness, screenHeight, customWhite);
}