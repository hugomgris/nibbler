#include "../../incs/IGraphic.hpp"
#include <iostream>

class TestLib : public IGraphic {
	public:
		void init(int width, int height) override {
			std::cout << "[TestLib] init(" << width << ", " << height << ")" << std::endl;
		}

		void render(const GameState &state) override {
			std::cout << "[TestLib] render: snake length - "
				<< state.snake.length << std::endl;
			
				if (state.gameOver)
					std::cout << "[TestLib] GAME OVER" << std::endl;
		}

		Input pollInput()  override{
			return Input::None;
		}
};

extern "C" IGraphic *createGraphic() {
	return new TestLib();
}

extern "C" void destroyGraphic(IGraphic *g) {
	delete g;
}