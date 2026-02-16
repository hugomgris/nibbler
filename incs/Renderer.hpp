#pragma once
#include "Snake.hpp"
#include "Food.hpp"
#include "DataStructs.hpp"
#include "Input.hpp"
#include "RaylibColors.hpp"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <iostream>
#include <array>
#include <memory>
#include <vector>
#include <chrono>

// Forward declarations
class ParticleSystem;
class AnimationSystem;
class MenuSystem;

class Renderer {
	private:
		int			screenWidth;
		int			screenHeight;
	
		// 3D pipeline
		Camera3D	camera3D;
		float		cubeSize;
		float		cameraSize;
		float 		menuFov;
		float		customFov;

	// 2D pipleine
	Camera2D	camera2D;
	float		squareSize;
	float		separator;
	int			gridWidth;
	int			gridHeight;
	
	// 2D layout calculations
	int			borderThickness;
	int			arenaWidth;    // Total arena including borders
	int			arenaHeight;
	int			arenaOffsetX;  // Center offset in screen
	int			arenaOffsetY;
	int			gameAreaX;     // Where game grid starts (after border)
	int			gameAreaY;
	
	// 2D tail tracking (for particle trails)
	struct TailState {
		float lastX = 0.0f;
		float lastY = 0.0f;
		bool isFirstFrame = true;
	};
	TailState snakeA_tailState;
	TailState snakeB_tailState;
	
	// Food tracking (for explosion particles)
	int lastFoodX = -1;
	int lastFoodY = -1;
	
	void calculate2DLayout();  // Helper to compute arena positioning
	Vector2 gridToScreen2D(int gridX, int gridY) const;  // Convert grid coords to screen coords
	
		float	accumulatedTime;
		
	public:
		Renderer();
		Renderer(const Renderer &other) = delete;
		Renderer &operator=(const Renderer &other) = delete;
		~Renderer();

		float getCubeSize() const { return cubeSize; }
		float getSquareSize() const { return squareSize; }
		float getSeparator() const { return separator; }
		Camera3D& getCamera3D() { return camera3D; }
		Camera2D& getCamera2D() { return camera2D; }
		float& getAccumulatedTime() { return accumulatedTime; }

		// 3D pipeline
		void setupCamera3D(); 
		void drawGroundPlane3D() ;
		void drawWalls3D();
		void drawSnake3D(const Snake* snake, Color hidden,
			Color lightFront,  Color lightTop, Color lightSide,
			Color darkFront, Color darkTop, Color darkSide);
		void drawFood3D(const Food* food);
		void drawCubeCustomFaces(Vector3 position, float width, float height, float length,
								Color front, Color back, Color top, Color bottom, Color right, Color left);
		
	
	// 2D pipeline
	void drawSnake2D(const Snake* snake, Color color, ParticleSystem& particles, TailState& tailState);
	void drawFood2D(const Food* food, ParticleSystem& particles);
	void drawBorderFullscreen(int thickness);  // Full-screen border (menu/game over)
	void drawBorderCentered(int thickness);     // Centered arena border (2D game)
	public:
		void init(int width, int height);
		void render3D(const GameState& state, float deltaTime);
		void render2D(const GameState& state, float deltaTime, ParticleSystem& particles, AnimationSystem& animations, Color color);
};
