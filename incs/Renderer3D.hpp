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

class Renderer3D {
private:
	float	cubeSize;
	float	cameraSize;
	//float 	menuFov;
	float	customFov;
	float	separator;
	int		gridWidth;
	int		gridHeight;
	int		screenWidth;
	int		screenHeight;

	float	accumulatedTime;

	Camera3D	camera;
	Camera2D	camera2D;
	
public:
	Renderer3D();
	Renderer3D(const Renderer3D &other) = delete;
	Renderer3D &operator=(const Renderer3D &other) = delete;
	~Renderer3D();

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
	void drawBorder(int thickness);  // Menu/GameOver border

public:
	void init(int width, int height);
	void render(const GameState& state, float deltaTime);
};
