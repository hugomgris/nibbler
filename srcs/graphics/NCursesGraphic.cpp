#include "../../incs/IGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/colors.h"
#include <ncurses.h>
#include <iostream>

class NCursesGraphic : public IGraphic {
private:
	int		width, height;
	WINDOW	*gameWindow;
	
public:
	NCursesGraphic() : width(0), height(0), gameWindow(nullptr) {}

	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	
	void init(int w, int h) override {
		width = w;
		height = h;
		
		std::cout << BBLU << "[NCurses] Initialized: " << width << "x" << height << RESET << std::endl;
		
		initscr();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);
		curs_set(0);
		
		start_color();
		init_pair(1, COLOR_GREEN, COLOR_BLACK);
		init_pair(2, COLOR_RED, COLOR_BLACK);
		init_pair(3, COLOR_BLACK, COLOR_BLACK);
		
		// +2 for borders (1 left, 1 right, 1 top, 1 bottom)
		gameWindow = newwin(
			height + 2,  // Height + borders
			width + 2,   // Width + borders
			1,           // Y position (row 1)
			1            // X position (col 1)
		);
		
		keypad(gameWindow, TRUE);
		nodelay(gameWindow, TRUE);

		bkgd(COLOR_PAIR(0));
		clear();
		refresh();
	}
	
	void render(const GameState& state) override {
		werase(gameWindow);
		
		box(gameWindow, 0, 0);
		
		wattron(gameWindow, COLOR_PAIR(3));
		for (int y = 1; y <= height; ++y) {
			for (int x = 1; x <= width; ++x) {
				mvwaddch(gameWindow, y, x, ' ');
			}
		}
		wattroff(gameWindow, COLOR_PAIR(3));
		
		// Draw snake
		wattron(gameWindow, COLOR_PAIR(1));
		for (int i = 0; i < state.snake->getLength(); ++i) {
			mvwaddch(
				gameWindow,
				state.snake->getSegments()[i].y + 1,
				state.snake->getSegments()[i].x + 1,
				'O'
			);
		}
		wattroff(gameWindow, COLOR_PAIR(1));
		
		// Draw food
		wattron(gameWindow, COLOR_PAIR(2));
		mvwaddch(gameWindow, state.food->getPosition().y + 1, state.food->getPosition().x + 1, '*');
		wattroff(gameWindow, COLOR_PAIR(2));
		
		// Double buffer: stage stdscr first, then gameWindow
		wnoutrefresh(stdscr);
		wnoutrefresh(gameWindow);
		doupdate();
	}
	
	Input pollInput() override {
		int ch = getch();
		switch (ch) {
			case KEY_UP:    return Input::Up;
			case KEY_DOWN:  return Input::Down;
			case KEY_LEFT:  return Input::Left;
			case KEY_RIGHT: return Input::Right;
			case 'q':       return Input::Quit;
			case 27 :		return Input::Quit; // ESC key
			case '1':       return Input::SwitchLib1;
			case '2':       return Input::SwitchLib2;
			case '3':       return Input::SwitchLib3;
			default:        return Input::None;
		}
	}
	
	~NCursesGraphic() {
		if (gameWindow) delwin(gameWindow);  // Delete subwindow first
		endwin();
		std::cout << BBLU << "[NCurses] Destroyed" << RESET << std::endl;
	}
};

extern "C" IGraphic* createGraphic() {
	return new NCursesGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
