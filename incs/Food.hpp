#pragma once
#include "DataStructs.hpp"
#include "Utils.hpp"

class Food {
	private:
		Vec2		_position;
		int			_hLimit;
		int			_vLimit;
		const char	*_foodChar;

	public:
		Food() = delete;
		Food(Vec2 position, int width, int height);
		
		Food(const Food &other);
		Food &operator=(const Food &other);

		bool replace(Vec2 newPos);

		Vec2 getPosition() const;
		const char* getFoodChar() const;
};