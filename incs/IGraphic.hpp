#pragma once
#include "DataStructs.hpp"
#include "Input.hpp"

class IGraphic {
	public:
		virtual ~IGraphic() = default;

		virtual void init(int width, int height) = 0;
		virtual void render(const GameState &state, float deltaTime) = 0;
		virtual void renderMenu(const GameState& state, float deltaTime) = 0;
		virtual void renderGameOver(const GameState& state, float deltaTime) = 0;
		virtual Input pollInput() = 0;
};

extern "C" {
	IGraphic *createGraphic();
	void destroyGraphic(IGraphic*);
}