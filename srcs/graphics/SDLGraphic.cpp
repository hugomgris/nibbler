#include "../../incs/SDLGraphic.hpp"

SDLGraphic::SDLGraphic() : window(nullptr), renderer(nullptr), cellSize(50) {}

SDLGraphic::~SDLGraphic() {
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << BRED << "[SDL2] Destroyed" << RESET << std::endl;
}

void SDLGraphic::init(int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
		return;
	}
	
	window = SDL_CreateWindow(
		"Nibbler - SDL2",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width * cellSize,
		height * cellSize,
		SDL_WINDOW_SHOWN
	);
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	std::cout << BRED << "[SDL2] Initialized: " << width << "x" << height << RESET << std::endl;
}
	
void SDLGraphic::render(const GameState& state) {
	// Black background
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	// Draw snake
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	for (int i = 0; i < state.snake->getLength(); ++i) {
		SDL_Rect rect = {
			state.snake->getSegments()[i].x * cellSize,
			state.snake->getSegments()[i].y * cellSize,
			cellSize,
			cellSize
		};
		SDL_RenderFillRect(renderer, &rect);
	}
	
	// Draw food
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect foodRect = {
		state.food->getPosition().x * cellSize,
		state.food->getPosition().y * cellSize,
		cellSize,
		cellSize
	};
	SDL_RenderFillRect(renderer, &foodRect);
	
	SDL_RenderPresent(renderer);
}
	
Input SDLGraphic::pollInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return Input::Quit;
		
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_UP:		return Input::Up;
				case SDLK_DOWN:		return Input::Down;
				case SDLK_LEFT:		return Input::Left;
				case SDLK_RIGHT:	return Input::Right;
				case SDLK_q:		return Input::Quit;
				case SDLK_ESCAPE:	return Input::Quit;
				case SDLK_1:		return Input::SwitchLib1;
				case SDLK_2:		return Input::SwitchLib2;
				case SDLK_3:		return Input::SwitchLib3;
			}
		}
	}
	return Input::None;
}