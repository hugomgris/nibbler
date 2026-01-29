#include "../incs/Utils.hpp"

static std::mt19937& getGenerator() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return gen;
}

int Utils::getRandomInt(int max)
{
	std::uniform_int_distribution<> distr(0, max);
	return distr(getGenerator());
}

int Utils::getRandomRangeInt(int min, int max)
{
	std::uniform_int_distribution<> distr(min, max);
	return distr(getGenerator());
}

Vec2 Utils::getRandomVec2(int xMax, int yMax)
{
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