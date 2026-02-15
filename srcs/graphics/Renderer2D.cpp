#include "../../incs/Renderer2D.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include "../../incs/RaylibColors.hpp"
#include "../../incs/ParticleSystem.hpp"
#include "../../incs/TextSystem.hpp"
#include "../../incs/AnimationSystem.hpp"
#include "../../incs/MenuSystem.hpp"
#include <rlgl.h>  // For low-level drawing functions (rlPushMatrix, rlBegin, etc.)

renderer2D::renderer2D() {}

renderer2D::~renderer2D() {
	CloseWindow();
	std::cout << BYEL << "[Raylib 2D] Destroyed" << RESET << std::endl;
}

