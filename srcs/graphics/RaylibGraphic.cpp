#include "../../incs/IGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/RaylibGraphic.hpp"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

RaylibGraphic::RaylibGraphic() :
	cubeSize(2.0f),
	gridWidth(0),
	gridHeight(0),
	screenWidth(1920),
	screenHeight(1080),
	accumulatedTime(0.0f) {}

RaylibGraphic::~RaylibGraphic() {
		UnloadTexture(grainTexture);
		CloseWindow();
		std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
	}

void RaylibGraphic::drawCubeCustomFaces(Vector3 position, float width, float height, float length,
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
			Vector3 position = {
				x * cubeSize,
				0.0f,
				z * cubeSize
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

void RaylibGraphic::drawWalls() {
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
	float yPos = cubeSize;
	
	for (int i = 0; i < snake->getLength(); i++) {
		const Vec2& segment = snake->getSegments()[i];
		
		Vector3 position = {
			segment.x * cubeSize,
			yPos,
			segment.y * cubeSize
		};
		
		// Head is full size, body is 80% size
		float size = (i == 0) ? cubeSize : cubeSize * 0.8f;
		if (i > 0) position.y *= 0.8f;  // Adjust Y position for body
		
		// Checkerboard pattern for all segments
		if (i % 2 == 0) {
			drawCubeCustomFaces(position, size, size, size,
			                    snakeLightFront, snakeHidden, snakeLightTop, snakeHidden, snakeLightSide, snakeHidden);
		} else {
			drawCubeCustomFaces(position, size, size, size,
			                    snakeDarkFront, snakeHidden, snakeDarkTop, snakeHidden, snakeDarkSide, snakeHidden);
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
	
	// Pulsing effect using controlled time (freezes when paused)
	float pulse = 1.0f + sinf(accumulatedTime * 3.0f) * 0.1f;

	drawCubeCustomFaces(position, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse,
						foodFront, foodHidden, foodTop, foodHidden, foodSide, foodHidden);
}

void RaylibGraphic::drawNoiseGrain() {
	float offsetX = sinf(accumulatedTime * 0.5f) * 10.0f - 20.0f;
	float offsetY = cosf(accumulatedTime * 0.3f) * 10.0f - 20.0f;
	
	DrawTextureEx(grainTexture, (Vector2){ offsetX, offsetY }, 0.0f, 1.0f, (Color){ 255, 255, 255, 20 });
}

void RaylibGraphic::init(int width, int height) {
	gridWidth = width;
	gridHeight = height;
	
	InitWindow(screenWidth, screenHeight, "Nibbler 3D - Raylib");
	SetTargetFPS(60);
	
	setupCamera();
	
	// Grain Texture
	int paddedWidth = screenWidth + 40;   // +40 pixels (±20 for oscillation)
	int paddedHeight = screenHeight + 40;
	Image grainImage = GenImageWhiteNoise(paddedWidth, paddedHeight, 0.75f);
	grainTexture = LoadTextureFromImage(grainImage);
	UnloadImage(grainImage);
	
	std::cout << BYEL << "[Raylib 3D] Initialized: " << width << "x" << height << RESET << std::endl;
}

void RaylibGraphic::render(const GameState& state, float deltaTime){
	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }

	BeginDrawing();
	ClearBackground(customBlack);
	
	BeginMode3D(camera);
	
	drawGroundPlane();
	//drawWalls();
	drawSnake(state.snake);
	drawFood(state.food);
	
	// Optional: Draw grid lines for debugging
	// DrawGrid(gridWidth, cubeSize);
	
	EndMode3D();
	
	DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, customWhite);
	DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, customWhite);
	DrawFPS(screenWidth - 95, 10);

	if (state.isPaused) {
        //DrawOutlinedText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customWhite, 1, customBlack);
		DrawText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customBlack);
    }
	
	// Post Processing
	drawNoiseGrain();
	
	EndDrawing();
}

void RaylibGraphic::renderMenu(const GameState& state) {
	(void)state;
	
	BeginDrawing();
	ClearBackground(customBlack);
	
	// TODO: Implement proper Raylib menu screen
	DrawText("NIBBLER", screenWidth/2 - 150, screenHeight/2 - 100, 60, customWhite);
	DrawText("Press ENTER to start", screenWidth/2 - 150, screenHeight/2, 30, customWhite);
	
	EndDrawing();
}

void RaylibGraphic::renderGameOver(const GameState& state) {
	BeginDrawing();
	ClearBackground(customBlack);
	
	// TODO: Implement proper Raylib game over screen
	DrawText("GAME OVER", screenWidth/2 - 150, screenHeight/2 - 50, 50, customWhite);
	
	char scoreText[50];
	snprintf(scoreText, sizeof(scoreText), "Score: %d", state.score);
	DrawText(scoreText, screenWidth/2 - 80, screenHeight/2 + 20, 30, customWhite);
	DrawText("Press ENTER to restart", screenWidth/2 - 150, screenHeight/2 + 80, 25, customWhite);
	
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
	if (IsKeyPressed(KEY_SPACE))	return Input::Pause;
	if (IsKeyPressed(KEY_ENTER))	return Input::Enter;
	if (IsKeyPressed(KEY_KP_ENTER))	return Input::Enter;
	
	if (WindowShouldClose())		return Input::Quit;
	
	return Input::None;
}

void RaylibGraphic::DrawOutlinedText(const char *text, int posX, int posY, int fontSize, Color color, int outlineSize, Color outlineColor) {
    DrawText(text, posX - outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX - outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX, posY, fontSize, color);
}