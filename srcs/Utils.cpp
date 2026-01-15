#include "../incs/Utils.hpp"

Vec2 Utils::getRandomVec2(int xMax, int yMax)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> xDistr(0, xMax - 1);
	std::uniform_int_distribution<> yDistr(0, yMax - 1);

	return Vec2{xDistr(gen), yDistr(gen)};
}