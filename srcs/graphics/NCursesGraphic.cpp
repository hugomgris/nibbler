#include "../../incs/IGraphic.hpp"
#include "../../incs/colors.h"
#include <ncurses.h>
#include <iostream>

class NCursesGraphic : public IGraphic {
private:
	int width, height;
	
public:
	NCursesGraphic() : width(0), height(0) {}

	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	
	void init(int w, int h) override {
		width = w;
		height = h;
		
		initscr();              // Initialize ncurses
		cbreak();               // Disable line buffering
		noecho();               // Don't echo input
		keypad(stdscr, TRUE);   // Enable arrow keys
		nodelay(stdscr, TRUE);  // Non-blocking input
		curs_set(0);            // Hide cursor
		
		start_color();
		init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Snake
		init_pair(2, COLOR_RED, COLOR_BLACK);    // Food
		
		std::cout << BBLU << "[NCurses] Initialized: " << width << "x" << height << RESET << std::endl;
	}
	
	void render(const GameState& state) override {
		clear();
		
		// Draw border
		box(stdscr, 0, 0);
		
		// Draw snake
		attron(COLOR_PAIR(1));
		for (size_t i = 0; i < state.snake.length; ++i) {
			mvaddch(
				state.snake.segments[i].y + 1,
				state.snake.segments[i].x + 1,
				'O'
			);
		}
		attroff(COLOR_PAIR(1));
		
		// Draw food
		attron(COLOR_PAIR(2));
		mvaddch(state.food.position.y + 1, state.food.position.x + 1, '*');
		attroff(COLOR_PAIR(2));
		
		refresh();
	}
	
	Input pollInput() override {
		int ch = getch();
		switch (ch) {
			case KEY_UP:    return Input::Up;
			case KEY_DOWN:  return Input::Down;
			case KEY_LEFT:  return Input::Left;
			case KEY_RIGHT: return Input::Right;
			case 'q':       return Input::Quit;
			case '1':       return Input::SwitchLib1;
			case '2':       return Input::SwitchLib2;
			case '3':       return Input::SwitchLib3;
			default:        return Input::None;
		}
	}
	
	~NCursesGraphic() {
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
