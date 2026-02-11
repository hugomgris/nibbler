#pragma once
#include "Snake.hpp"
#include "Food.hpp"
#include "DataStructs.hpp"
#include "Input.hpp"
#include "colors.h"
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

struct BorderLine {
	float progress;        // 0 = at center, 1 = at edge
	float age;
	
	BorderLine() : progress(0.0f), age(0.0f) {}
};

class Renderer {
private:
	float	cubeSize;
	float	cameraSize;
	float 	menuFov;
	float	customFov;
	float	separator;
	int		gridWidth;
	int		gridHeight;
	int		screenWidth;
	int		screenHeight;

	float	accumulatedTime;

	Camera3D	camera;
	Camera2D	camera2D;

	// Postprocessing noise system
	static const int							GRAIN_TEXTURE_COUNT = 8;
	std::array<Texture2D, GRAIN_TEXTURE_COUNT>	grainTextures;
	int											currentGrainFrame;
	float										grainFrameTimer;
	float										grainFrameInterval;
	
	// Tunnel effect system
	std::vector<BorderLine>						borderLines;
	std::chrono::high_resolution_clock::time_point lastSpawnTime;
	float										spawnInterval;
	float										animationSpeed;
	bool										enableTunnelEffect;
	
	// Colors
	Color customWhite = { 255, 248, 227, 255};				// Warm off-white (cream)
	Color customGray = { 125, 125, 125, 255};				// Mid gray
	Color customBlack = { 23, 23, 23, 255};					// Deep navy black
	
	// Logo colors - Dark segments (darker overall, balanced contrast)
	Color logoDarkTop = { 180, 175, 160, 255 };				// Medium-dark cream/beige (brightest of dark set)
	Color logoDarkFront = { 70, 67, 58, 255 };				// Dark brown-gray (not too close to black)
	Color logoDarkSide = { 125, 120, 108, 255 };			// Medium warm gray (between top and front)

	// Logo colors - Light segments (lighter overall, more contrast)
	Color logoLightTop = customWhite;						// Brightest (255, 248, 227)
	Color logoLightFront = { 170, 165, 148, 255 };			// Medium beige (darker for more contrast)
	Color logoLightSide = { 215, 208, 188, 255 };			// Light beige (between top and front)

	// Ground colors - Light squares
	Color groundLightTop = customWhite;
	Color groundLightFront = { 26, 64, 96, 255 };			// Blue
	Color groundLightSide = { 254, 74, 81, 255 };			// Red
	Color groundHidden = customBlack;
	
	// Ground colors - Dark squares
	Color groundDarkTop = { 200, 195, 178, 255 };			// Darker cream/beige
	Color groundDarkFront = { 18, 45, 68, 255 };			// Darker blue
	Color groundDarkSide = { 180, 52, 58, 255 };			// Darker red
	
	// Wall colors
	Color wallColor = { 147, 112, 219, 255 };				// Medium purple
	Color wallColorFade = { 147, 112, 219, 120 };			// Faded purple (transparent)
	
	// Snake A colors - Blue shades (matches ground blue)
	// Light segments
	Color snakeALightTop = { 135, 206, 250, 255 };			// Light sky blue (brightest)
	Color snakeALightFront = { 26, 64, 96, 255 };			// Ground blue (darker)
	Color snakeALightSide = { 70, 130, 180, 255 };			// Steel blue (lighter than front)
	
	// Dark segments
	Color snakeADarkTop = { 70, 130, 180, 255 };			// Steel blue
	Color snakeADarkFront = { 18, 45, 68, 255 };			// Ground dark blue (darker)
	Color snakeADarkSide = { 26, 64, 96, 255 };				// Ground blue (lighter than front)
	Color snakeAHidden = customBlack;
	
	// Snake B colors - Golden/Orange shades (more saturated for better contrast)
	// Light segments
	Color snakeBLightTop = { 255, 215, 0, 255 };			// Pure golden yellow (brightest)
	Color snakeBLightFront = { 218, 112, 21, 255 };			// Peru orange (darker, more orange)
	Color snakeBLightSide = { 255, 165, 0, 255 };			// Pure orange (lighter than front)
	
	// Dark segments
	Color snakeBDarkTop = { 255, 165, 0, 255 };				// Pure orange
	Color snakeBDarkFront = { 184, 85, 15, 255 };			// Dark orange (darker)
	Color snakeBDarkSide = { 218, 112, 21, 255 };			// Peru orange (lighter than front)
	Color snakeBHidden = customBlack;
	
	// Snake AI colors - Green shades (darker for better contrast)
	// Light segments
	Color snakeAILightTop = { 50, 205, 50, 255 };			// Lime green (bright but darker than before)
	Color snakeAILightFront = { 34, 139, 34, 255 };			// Forest green (darker)
	Color snakeAILightSide = { 46, 179, 113, 255 };			// Medium sea green (lighter than front)
	
	// Dark segments
	Color snakeAIDarkTop = { 46, 179, 113, 255 };			// Medium sea green
	Color snakeAIDarkFront = { 25, 100, 60, 255 };			// Dark forest green (much darker)
	Color snakeAIDarkSide = { 34, 139, 34, 255 };			// Forest green (lighter than front)
	Color snakeAIHidden = customBlack;
	
	// Food colors - Red shades (based on ground red)
	Color foodTop = { 255, 120, 120, 255 };					// Light coral red (brightest)
	Color foodFront = { 180, 52, 58, 255 };					// Ground dark red (darker)
	Color foodSide = { 254, 74, 81, 255 };					// Ground red (lighter than front)
	Color foodHidden = customBlack; 
	
public:
	Renderer();
	Renderer(const Renderer &other) = delete;
	Renderer &operator=(const Renderer &other) = delete;
	~Renderer();

	float getCubeSize() const;
    float getSeparator() const;
    Camera3D& getCamera();
    float& getAccumulatedTime();

	void setupCamera(); 
	void drawGroundPlane() ;
	void drawWalls();
	void drawSnake(const Snake* snake, Color hidden,
		Color lightFront,  Color lightTop, Color lightSide,
		Color darkFront, Color darkTop, Color darkSide);
	void drawFood(const Food* food);
	void drawCubeCustomFaces(Vector3 position, float width, float height, float length,
	                         Color front, Color back, Color top, Color bottom, Color right, Color left);
	void drawNoiseGrain();  // Post Processing
	void drawBorder(int thickness);  // Menu/GameOver border
	
	// Tunnel effect
	float easeInQuad(float t);
	void updateTunnelEffect(float deltaTime);
	void renderTunnelEffect();

public:
	void init(int width, int height);
	void render(const GameState& state, float deltaTime);
	void renderMenu(const GameState &state, float deltaTime, ParticleSystem& particles, 
	                class TextSystem& textSystem);
	void renderGameOver(const GameState &state, float deltaTime, ParticleSystem& particles,
	                    class TextSystem& textSystem);
	Input pollInput();
};
