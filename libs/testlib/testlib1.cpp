#include "../../incs/IGraphic.hpp"
#include "../../incs/colors.h"
#include <iostream>

class TestLib1 : public IGraphic {
	public:
		void init(int width, int height) override {
			std::cout << BRED << "[TestLib-1]" << RESET <<  " init(" << width << ", " << height << ")" << std::endl;
		}

		void render(const GameState &state) override {
			std::cout << BRED << "[TestLib-1]" << RESET << " render: snake length - "
				<< state.snake.length << std::endl;
			
				if (state.gameOver)
					std::cout  << BRED << "[TestLib-1]" << RESET << " GAME OVER" << std::endl;
		}

		Input pollInput()  override{
			// This should be nonblocking, but for testing purposes it's ok like this
			return Input::None;
		}
};

extern "C" IGraphic *createGraphic() {
	return new TestLib1();
}

extern "C" void destroyGraphic(IGraphic *g) {
	delete g;
}