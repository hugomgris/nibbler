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

Vec2 Utils::getRandomSpawnPosition(int width, int height)
{
	// safe spawn zone away from borders
	int minX = 8;
	int maxX = width - 8;
	int minY = 8;
	int maxY = height - 8;
	
	// Calculate center row and/or column based on odd/even dimensions
	int centerX = width / 2;
	int centerY = height / 2;
	bool hasOddWidth = (width % 2 == 1);
	bool hasOddHeight = (height % 2 == 1);
	
	int x, y;
	
	// Keep generating until a non-center position is found if the dimension is odd
	do {
		x = getRandomRangeInt(minX, maxX);
		y = getRandomRangeInt(minY, maxY);
	} while ((hasOddWidth && x == centerX) || (hasOddHeight && y == centerY));
	
	return Vec2{x, y};
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