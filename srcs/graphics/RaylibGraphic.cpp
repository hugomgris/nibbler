#include "../../incs/IGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/RaylibGraphic.hpp"

RaylibGraphic::RaylibGraphic() :
	cubeSize(2.0f),
	gridWidth(0),
	gridHeight(0),
	screenWidth(1920),
	screenHeight(1080) {}

RaylibGraphic::~RaylibGraphic() {
		CloseWindow();
		std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
	}

void RaylibGraphic::setupCamera() {
	float centerX = (gridWidth * cubeSize) / 2.0f;
	float centerZ = (gridHeight * cubeSize) / 2.0f;
	
	// Calculate diagonal distance to ensure entire grid fits
	float diagonal = sqrtf(gridWidth * gridWidth + gridHeight * gridHeight) * cubeSize;
	float distance = diagonal * 2.2f;  // 20% padding
	
	// Standard isometric angles: 35.264° elevation, 45° rotation
	float elevation = 35.264f * DEG2RAD;  // Classic isometric angle
	float rotation = 45.0f * DEG2RAD;
	
	camera.position = (Vector3){ 
		centerX + distance * cosf(rotation) * cosf(elevation),
		distance * sinf(elevation),
		centerZ + distance * sinf(rotation) * cosf(elevation)
	};
	
	camera.target = (Vector3){ centerX, cubeSize * 2, centerZ }; // "* 3" is there to adjust the centering of the scene
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 60.0f; // Can be used in Ortho mode to tweak the camera zoom
	camera.projection = CAMERA_ORTHOGRAPHIC;
}

void RaylibGraphic::drawGroundPlane() {
	for (int z = 0; z < gridHeight; z++) {
		for (int x = 0; x < gridWidth; x++) {
			Color squareColor = ((x + z) % 2 == 0) ? lightSquare : darkSquare;
			
			Vector3 position = {
				x * cubeSize,
				0.0f,
				z * cubeSize
			};
			
			DrawCube(position, cubeSize, cubeSize, cubeSize, squareColor);
		}
	}
}

void RaylibGraphic::drawWalls() {
	// Stack of 3 cubes for walls (temporarily)
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

void RaylibGraphic::drawSnake(const Snake* snake) {
	// Draw snake at level 1 (one cube above ground)
	float yPos = cubeSize;  // Center of level 1
	
	for (int i = 0; i < snake->getLength(); i++) {
		const Vec2& segment = snake->getSegments()[i];
		
		Vector3 position = {
			segment.x * cubeSize,
			yPos,
			segment.y * cubeSize
		};
		
		if (i == 0) {
			DrawCube(position, cubeSize, cubeSize, cubeSize, headColor);
		} else {
			// Body pieces set up to 80% of cube size
			position.y = position.y * .8f;
			(i % 2 == 0) ?
				DrawCube(position, cubeSize *.8f, cubeSize *.8f, cubeSize *.8f, BodyColor_1) :
				DrawCube(position, cubeSize *.8f, cubeSize *.8f, cubeSize *.8f, BodyColor_2) ;
		}
	}
}

void RaylibGraphic::drawFood(const Food* food) {
	float yPos = cubeSize;
	
	Vec2 foodPos = food->getPosition();
	Vector3 position = {
		foodPos.x * cubeSize,
		yPos,
		foodPos.y * cubeSize
	};
	
	// Pulsing effect for food
	float pulse = 1.0f + sinf(GetTime() * 3.0f) * 0.1f;
	
	DrawCube(position, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, foodColor);
}

void RaylibGraphic::init(int width, int height) {
	gridWidth = width;
	gridHeight = height;
	
	InitWindow(screenWidth, screenHeight, "Nibbler 3D - Raylib");
	SetTargetFPS(60);
	
	setupCamera();
	
	std::cout << BYEL << "[Raylib 3D] Initialized: " << width << "x" << height << RESET << std::endl;
}

void RaylibGraphic::render(const GameState& state){
	BeginDrawing();
	ClearBackground(RAYWHITE);
	
	BeginMode3D(camera);
	
	// Draw in order: ground -> walls -> snake -> food
	drawGroundPlane();
	//drawWalls();
	drawSnake(state.snake);
	drawFood(state.food);
	
	// Optional: Draw grid lines for debugging
	// DrawGrid(gridWidth, cubeSize);
	
	EndMode3D();
	
	// 2D HUD overlay
	DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, DARKGRAY);
	DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, DARKGRAY);
	DrawFPS(screenWidth - 95, 10);
	
	EndDrawing();
}

Input RaylibGraphic::pollInput() {
	if (IsKeyPressed(KEY_UP))		return Input::Up;
	if (IsKeyPressed(KEY_DOWN))		return Input::Down;
	if (IsKeyPressed(KEY_LEFT))		return Input::Left;
	if (IsKeyPressed(KEY_RIGHT))	return Input::Right;
	if (IsKeyPressed(KEY_Q))		return Input::Quit;
	if (IsKeyPressed(KEY_ESCAPE))	return Input::Quit;
	if (IsKeyPressed(KEY_ONE))		return Input::SwitchLib1;
	if (IsKeyPressed(KEY_TWO))		return Input::SwitchLib2;
	if (IsKeyPressed(KEY_THREE))	return Input::SwitchLib3;
	
	if (WindowShouldClose())		return Input::Quit;
	
	return Input::None;
}
