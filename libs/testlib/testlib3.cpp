#include "../../incs/IGraphic.hpp"
#include "../../incs/colors.h"
#include <iostream>

class TestLib3 : public IGraphic {
	public:
		void init(int width, int height) override {
			std::cout << BBLU << "[TestLib-3]" << RESET <<  " init(" << width << ", " << height << ")" << std::endl;
		}

		void render(const GameState &state) override {
			std::cout << BBLU << "[TestLib-3]" << RESET << " render: snake length - "
				<< state.snake.length << std::endl;
			
				if (state.gameOver)
					std::cout  << BBLU << "[TestLib-3]" << RESET << " GAME OVER" << std::endl;
		}

		Input pollInput()  override{
			// This should be nonblocking, but for testing purposes it's ok like this
			return Input::None;
		}
};

extern "C" IGraphic *createGraphic() {
	return new TestLib3();
}

extern "C" void destroyGraphic(IGraphic *g) {
	delete g;
}