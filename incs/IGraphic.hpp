#pragma once
#include "GameState.hpp"
#include "Input.hpp"

class IGraphic {
	public:
		virtual ~IGraphic() = default;

		virtual void init(int width, int height) = 0;
		virtual void render(const GameState &state) = 0;
		virtual Input pollInput() = 0;
};

extern "C" {
	IGraphic *createGraphic();
	void destroyGraphic(IGraphic*);
}