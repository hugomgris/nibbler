#include "../../incs/Renderer.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/ParticleSystem.hpp"
#include "../../incs/TextSystem.hpp"
#include "../../incs/AnimationSystem.hpp"
#include "../../incs/MenuSystem.hpp"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

Renderer::Renderer() :
	cubeSize(2.0f),
	menuFov(50.0f),
	gridWidth(0),
	gridHeight(0),
	screenWidth(1920),
	screenHeight(1080),
	accumulatedTime(0.0f),
	currentGrainFrame(0),
	grainFrameTimer(0.0f),
	grainFrameInterval(0.05f) {
		separator = cubeSize * 2;
	}

Renderer::~Renderer() {
	// Unload all grain textures
	for (int i = 0; i < GRAIN_TEXTURE_COUNT; i++) {
		UnloadTexture(grainTextures[i]);
	}
	CloseWindow();
	std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
}

void Renderer::init(int width, int height) {
	gridWidth = width;
	gridHeight = height;
	
	InitWindow(screenWidth, screenHeight, "Nibbler 3D - Raylib");
	ToggleFullscreen();
	SetTargetFPS(60);
	
	setupCamera();
	
	// Setup 2D camera for UI rendering
	// Simple setup: no offset, target at origin, covers full screen in screen coordinates
	camera2D.offset = (Vector2){ 0.0f, 0.0f };
	camera2D.target = (Vector2){ 0.0f, 0.0f };
	camera2D.rotation = 0.0f;
	camera2D.zoom = 1.0f;
	
	// noise pattern generation
	for (int i = 0; i < GRAIN_TEXTURE_COUNT; i++) {
		// seeding for varaition
		Image grainImage = GenImageWhiteNoise(screenWidth, screenHeight, 0.75f);
		grainTextures[i] = LoadTextureFromImage(grainImage);
		UnloadImage(grainImage);
	}
	
	std::cout << BYEL << "[Raylib 3D] Initialized: " << width << "x" << height << RESET << std::endl;
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

void Renderer::setupCamera() {
	// Grid is now centered at origin (0, 0, 0)
	float centerX = 0.0f;
	float centerZ = 0.0f;
	
	float diagonal = sqrtf(gridWidth * gridWidth + gridHeight * gridHeight) * cubeSize;
	float distance = diagonal * 2.2f;  // 20% padding
	
	float elevation = 35.264f * DEG2RAD;  // Classic isometric angle
	float rotation = 45.0f * DEG2RAD;
	
	camera.position = (Vector3){ 
		centerX + distance * cosf(rotation) * cosf(elevation),
		distance * sinf(elevation),
		centerZ + distance * sinf(rotation) * cosf(elevation)
	};
	
	camera.target = (Vector3){ centerX, 0.0f, centerZ };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };

	cameraSize = static_cast<float>((gridWidth + gridHeight) / 2);
	customFov = 0.022619f * cameraSize * cameraSize + 0.198810f * cameraSize + 31.028571f;
	
	camera.fovy = customFov;
	camera.projection = CAMERA_ORTHOGRAPHIC;
}

void Renderer::drawGroundPlane() {
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
void Renderer::drawWalls() {
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

void Renderer::drawSnake(const Snake* snake, Color hidden,
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

void Renderer::drawFood(const Food* food) {
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

void Renderer::drawNoiseGrain() {
	DrawTextureEx(grainTextures[currentGrainFrame], (Vector2){ 0.0f, 0.0f }, 0.0f, 1.0f, (Color){ 255, 255, 255, 20 });
}

void Renderer::drawBorder(int thickness) {
	// Top
	DrawRectangle(0, 0, screenWidth, thickness, customWhite);
	// Bottom
	DrawRectangle(0, screenHeight - thickness, screenWidth, thickness, customWhite);
	// Left
	DrawRectangle(0, 0, thickness, screenHeight, customWhite);
	// Right
	DrawRectangle(screenWidth - thickness, 0, thickness, screenHeight, customWhite);
}


void Renderer::render(const GameState& state, float deltaTime){
	camera.fovy = customFov;
	
	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }
	
	// Update film grain pattern at regular intervals
	grainFrameTimer += deltaTime;
	if (grainFrameTimer >= grainFrameInterval) {
		grainFrameTimer = 0.0f;
		currentGrainFrame = GetRandomValue(0, GRAIN_TEXTURE_COUNT - 1);
	}

	BeginDrawing();
	ClearBackground(customBlack);
	
	BeginMode3D(camera);
	
	drawGroundPlane();
	//drawWalls();
	drawSnake(&state.snake_A, snakeAHidden, snakeALightFront, snakeALightTop, snakeALightSide, snakeADarkFront, snakeADarkTop, snakeADarkSide);
	if (state.config.mode == GameMode::MULTI)
		drawSnake(state.snake_B, snakeBHidden, snakeBLightFront, snakeBLightTop, snakeBLightSide, snakeBDarkFront, snakeBDarkTop, snakeBDarkSide);
	else if (state.config.mode == GameMode::AI)
		drawSnake(state.snake_B, snakeAIHidden, snakeAILightFront, snakeAILightTop, snakeAILightSide, snakeAIDarkFront, snakeAIDarkTop, snakeAIDarkSide);
	drawFood(&state.food);

	// DEBUG
	//DrawGrid(gridWidth, cubeSize);
	
	EndMode3D();
	
	DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, customWhite);
	DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, customWhite);
	DrawFPS(screenWidth - 95, 10);

	if (state.isPaused) {
		DrawText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customBlack); // this is horribly functional, need to design a proper pause menu system
    }
	
	// Post Processing
	drawNoiseGrain();
	
	EndDrawing();
}

Input Renderer::pollInput() {
	if (IsKeyPressed(KEY_UP))		return Input::Up_A;
	if (IsKeyPressed(KEY_DOWN))		return Input::Down_A;
	if (IsKeyPressed(KEY_LEFT))		return Input::Left_A;
	if (IsKeyPressed(KEY_RIGHT))	return Input::Right_A;
	if (IsKeyPressed(KEY_W))		return Input::Up_B;
	if (IsKeyPressed(KEY_S))		return Input::Down_B;
	if (IsKeyPressed(KEY_A))		return Input::Left_B;
	if (IsKeyPressed(KEY_D))		return Input::Right_B;
	if (IsKeyPressed(KEY_Q))		return Input::Quit;
	if (IsKeyPressed(KEY_ESCAPE))	return Input::Quit;
	if (IsKeyPressed(KEY_SPACE))	return Input::Pause;
	if (IsKeyPressed(KEY_ENTER))	return Input::Enter;
	if (IsKeyPressed(KEY_KP_ENTER))	return Input::Enter;
	
	if (WindowShouldClose())		return Input::Quit;
	
	return Input::None;
}

float Renderer::getCubeSize() const { return cubeSize; }
float Renderer::getSeparator() const { return separator; }
Camera3D& Renderer::getCamera() { return camera; }
float& Renderer::getAccumulatedTime() { return accumulatedTime; }