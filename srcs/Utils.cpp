#include "../incs/Utils.hpp"

int Utils::getRandomInt(int max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, max);
	return distr(gen);
}

Vec2 Utils::getRandomVec2(int xMax, int yMax)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	return Vec2{getRandomInt(xMax - 1), getRandomInt(yMax - 1)};
}

const char* Utils::getFoodChar(int idx)
{
	switch (idx)
	{
		case 0:
			return "Ở";
		case 1:
			return "Ợ";
		case 2:
			return "Ớ";
		case 3:
			return "Ộ";
		case 4:
			return "Ờ";
		case 5:
			return "Ọ";
		default:
			return "Ỡ";
	}
}