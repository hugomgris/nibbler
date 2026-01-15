#pragma once
#include "DataStructs.hpp"

class Food {
	private:
		Vec2	_position;
		int		_hLimit;
		int		_vLimit;

	public:
		Food() = delete;
		Food(Vec2 position, int width, int height);
		
		Food(const Food &other);
		Food &operator=(const Food &other);

		bool replace(Vec2 newPos);

		Vec2 getPosition();
};