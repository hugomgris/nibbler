#pragma once
#include "IGraphic.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Utils.hpp"
#include "colors.h"
#include <ncurses.h>
#include <locale.h>
#include <iostream>

class NCursesGraphic : public IGraphic {
private:
	int		width, height;
	WINDOW	*gameWindow;
	bool	isInitialized;
	
public:
	NCursesGraphic();
	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	~NCursesGraphic();
	
	void init(int w, int h) override;
	void render(const GameState& state) override;
	Input pollInput() override;
};

extern "C" IGraphic* createGraphic() {
	return new NCursesGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
