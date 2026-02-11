#include "../../incs/RaylibGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

RaylibGraphic::RaylibGraphic() :
	cubeSize(2.0f),
	menuFov(50.0f),
	gridWidth(0),
	gridHeight(0),
	screenWidth(1920),
	screenHeight(1080),
	accumulatedTime(0.0f),
	currentGrainFrame(0),
	grainFrameTimer(0.0f),
	grainFrameInterval(0.05f),
	spawnInterval(0.3f),
	animationSpeed(0.5f),
	enableTunnelEffect(true) {
		separator = cubeSize * 2;
		lastSpawnTime = std::chrono::high_resolution_clock::now();
	}

RaylibGraphic::~RaylibGraphic() {
	// Explicitly destroy helper classes before closing window
	// This ensures fonts/textures are unloaded while raylib context is still valid
	textRenderer.reset();
	titleHandler.reset();
	
	// Unload all grain textures
	for (int i = 0; i < GRAIN_TEXTURE_COUNT; i++) {
		UnloadTexture(grainTextures[i]);
	}
	CloseWindow();
	std::cout << BYEL << "[Raylib 3D] Destroyed" << RESET << std::endl;
}

void RaylibGraphic::init(int width, int height) {
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
		// Unique seeding for varaition
		Image grainImage = GenImageWhiteNoise(screenWidth, screenHeight, 0.75f);
		grainTextures[i] = LoadTextureFromImage(grainImage);
		UnloadImage(grainImage);
	}

	titleHandler = std::make_unique<RaylibTitleHandler>(*this);
	textRenderer = std::make_unique<RaylibTextRenderer>(*this);
	particleSystem = std::make_unique<RaylibParticleSystem>(screenWidth, screenHeight, 10, 0);
	particleSystem->setMaxDustDensity(30);
	particleSystem->setDustSpawnInterval(0.15f);
	
	std::cout << BYEL << "[Raylib 3D] Initialized: " << width << "x" << height << RESET << std::endl;
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

void RaylibGraphic::drawGroundPlane() {
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

void RaylibGraphic::drawSnake(const Snake* snake, Color hidden,
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

void RaylibGraphic::drawFood(const Food* food) {
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
	
	// Pulsing effect using controlled time (freezes when paused)
	float pulse = 1.0f + sinf(accumulatedTime * 3.0f) * 0.1f;

	drawCubeCustomFaces(position, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse,
						foodFront, foodHidden, foodTop, foodHidden, foodSide, foodHidden);
}

void RaylibGraphic::drawNoiseGrain() {
	DrawTextureEx(grainTextures[currentGrainFrame], (Vector2){ 0.0f, 0.0f }, 0.0f, 1.0f, (Color){ 255, 255, 255, 20 });
}

void RaylibGraphic::drawBorder(int thickness) {
	// Draw border at screen edges
	// Top border
	DrawRectangle(0, 0, screenWidth, thickness, customWhite);
	// Bottom border
	DrawRectangle(0, screenHeight - thickness, screenWidth, thickness, customWhite);
	// Left border
	DrawRectangle(0, 0, thickness, screenHeight, customWhite);
	// Right border
	DrawRectangle(screenWidth - thickness, 0, thickness, screenHeight, customWhite);
}

float RaylibGraphic::easeInQuad(float t) {
	return t * t;
}

void RaylibGraphic::updateTunnelEffect(float deltaTime) {
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

void RaylibGraphic::renderTunnelEffect() {
	if (!enableTunnelEffect || borderLines.empty()) return;

	// Border is now at screen edges with 25px thickness
	int borderThickness = 25;
	
	// Define starting rectangle - inset from border inner edge
	int contentInset = 60;  // Additional inset from border inner edge
	int startLeft = borderThickness + contentInset;
	int startTop = borderThickness + contentInset;
	int startRight = screenWidth - borderThickness - contentInset;
	int startBottom = screenHeight - borderThickness - contentInset;
	
	// Maximum travel distance (from start position to border inner edge)
	int maxTravelX = contentInset;
	int maxTravelY = contentInset;

	for (const auto& line : borderLines) {
		// Calculate current offset from start rectangle
		int offsetX = static_cast<int>(line.progress * maxTravelX);
		int offsetY = static_cast<int>(line.progress * maxTravelY);
		
		// Current rectangle coordinates
		int left = startLeft - offsetX;
		int top = startTop - offsetY;
		int right = startRight + offsetX;
		int bottom = startBottom + offsetY;
		int width = right - left;
		int height = bottom - top;
		
		unsigned char alpha = static_cast<unsigned char>(line.progress * 255);
		Color lineColor = {70, 130, 180, alpha};  // Light blue with fade

		int lineWidth = 2;

		// Draw expanding rectangle borders maintaining screen aspect ratio
		// Top border
		DrawRectangle(left, top, width, lineWidth, lineColor);
		
		// Bottom border
		DrawRectangle(left, bottom - lineWidth, width, lineWidth, lineColor);
		
		// Left border
		DrawRectangle(left, top, lineWidth, height, lineColor);
		
		// Right border
		DrawRectangle(right - lineWidth, top, lineWidth, height, lineColor);
	}
}


void RaylibGraphic::render(const GameState& state, float deltaTime){
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
        //DrawOutlinedText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customWhite, 1, customBlack);
		DrawText("PAUSED", screenWidth / 2 - 60, screenHeight / 2, 40, customBlack);
    }
	
	// Post Processing
	drawNoiseGrain();
	
	EndDrawing();
}

void RaylibGraphic::renderMenu(const GameState& state, float deltaTime) {
	getCamera().fovy = menuFov;

	if (!state.isPaused) {
        accumulatedTime += deltaTime;
    }
	
	// Update film grain pattern at regular intervals
	grainFrameTimer += deltaTime;
	if (grainFrameTimer >= grainFrameInterval) {
		grainFrameTimer = 0.0f;
		currentGrainFrame = GetRandomValue(0, GRAIN_TEXTURE_COUNT - 1);
	}
	
	// Update particles
	particleSystem->update(deltaTime);
	
	// Update tunnel effect
	updateTunnelEffect(deltaTime);
	
	BeginDrawing();
	ClearBackground(customBlack);

	BeginMode2D(camera2D);
	
	titleHandler->drawTitle();
	textRenderer->drawInstructions(state);
	
	// Render particles ON TOP of UI elements so they're visible
	particleSystem->render();
	
	// Render tunnel effect
	renderTunnelEffect();
	
	// Draw border on top
	drawBorder(25);

	EndMode2D();

	// drawNoiseGrain();  // Temporarily disabled to test particle visibility
	
	EndDrawing();
}

void RaylibGraphic::renderGameOver(const GameState& state, float deltaTime) {
	getCamera().fovy = menuFov;

	grainFrameTimer += deltaTime;
	if (grainFrameTimer >= grainFrameInterval) {
		grainFrameTimer = 0.0f;
		currentGrainFrame = GetRandomValue(0, GRAIN_TEXTURE_COUNT - 1);
	}
	
	// Update particles
	particleSystem->update(deltaTime);
	
	// Update tunnel effect
	updateTunnelEffect(deltaTime);
	
	BeginDrawing();
	ClearBackground(customBlack);

	BeginMode2D(camera2D);
	
	// Render particles
	particleSystem->render();
	
	titleHandler->drawGameover();
	
	if (state.config.mode != GameMode::SINGLE) {
		textRenderer->drawWinner(state);
	}
	
	textRenderer->drawScore(state);
	textRenderer->drawRetry(state);
	
	// Render tunnel effect
	renderTunnelEffect();
	
	// Draw border on top
	drawBorder(25);

	EndMode2D();

	drawNoiseGrain();
	
	EndDrawing();
}

Input RaylibGraphic::pollInput() {
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

float RaylibGraphic::getCubeSize() const { return cubeSize; }
float RaylibGraphic::getSeparator() const { return separator; }
Camera3D& RaylibGraphic::getCamera() { return camera; }
float& RaylibGraphic::getAccumulatedTime() { return accumulatedTime; }