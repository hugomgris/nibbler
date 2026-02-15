#include "../../incs/Renderer3D.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/RaylibColors.hpp"
#include "../../incs/ParticleSystem.hpp"
#include "../../incs/TextSystem.hpp"
#include "../../incs/AnimationSystem.hpp"
#include "../../incs/MenuSystem.hpp"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

Renderer3D::Renderer3D() :
	cubeSize(2.0f),
	menuFov(50.0f),
	gridWidth(0),
	gridHeight(0),
	screenWidth(1920),
	screenHeight(1080),
	accumulatedTime(0.0f) {
		separator = cubeSize * 2;
	}

Renderer3D::~Renderer3D() {
	CloseWindow();
	std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
}

void Renderer3D::init(int width, int height) {
	gridWidth = width;
	gridHeight = height;
	
	// IMPORTANT: Set VSync flag BEFORE InitWindow
	SetConfigFlags(FLAG_VSYNC_HINT);
	
	InitWindow(screenWidth, screenHeight, "Nibbler 3D - Raylib");
	ToggleFullscreen();
	SetTargetFPS(60);  // Lock to 60 FPS
	
	setupCamera();
	
	// Setup 2D camera for UI rendering
	// Simple setup: no offset, target at origin, covers full screen in screen coordinates
	camera2D.offset = (Vector2){ 0.0f, 0.0f };
	camera2D.target = (Vector2){ 0.0f, 0.0f };
	camera2D.rotation = 0.0f;
	camera2D.zoom = 1.0f;
	
	std::cout << BYEL << "[Raylib 3D] Initialized: " << width << "x" << height << RESET << std::endl;
}

void Renderer3D::drawCubeCustomFaces(Vector3 position, float width, float height, float length,
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

void Renderer3D::setupCamera() {
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

void Renderer3D::drawGroundPlane() {
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
void Renderer3D::drawWalls() {
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

void Renderer3D::drawSnake(const Snake* snake, Color hidden,
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

void Renderer3D::drawFood(const Food* food) {
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

void Renderer3D::drawBorder(int thickness) {
	// Top
	DrawRectangle(0, 0, screenWidth, thickness, customWhite);
	// Bottom
	DrawRectangle(0, screenHeight - thickness, screenWidth, thickness, customWhite);
	// Left
	DrawRectangle(0, 0, thickness, screenHeight, customWhite);
	// Right
	DrawRectangle(screenWidth - thickness, 0, thickness, screenHeight, customWhite);
}


void Renderer3D::render(const GameState& state, float deltaTime){
	// Update internal state (NOT drawing!)
	camera.fovy = customFov;
	
	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }
}

float Renderer3D::getCubeSize() const { return cubeSize; }
float Renderer3D::getSeparator() const { return separator; }
Camera3D& Renderer3D::getCamera() { return camera; }
float& Renderer3D::getAccumulatedTime() { return accumulatedTime; }