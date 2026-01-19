#pragma once
#include "../incs/DataStructs.hpp"
#include <random>

class Utils
{
	public:
		static int getRandomInt(int max);
		static int getRandomRangeInt(int min, int max);
		static Vec2 getRandomVec2(int xMax, int yMax);
		static const char* getFoodChar(int idx);
};