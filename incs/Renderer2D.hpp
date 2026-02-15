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

class renderer2D {
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
	
public:
	renderer2D();
	renderer2D(const renderer2D &other) = delete;
	renderer2D &operator=(const renderer2D &other) = delete;
	~renderer2D();

};
