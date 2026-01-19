#include "../../incs/IGraphic.hpp"
#include "../../incs/Snake.hpp"
#include "../../incs/Food.hpp"
#include "../../incs/Utils.hpp"
#include "../../incs/colors.h"
#include <ncurses.h>
#include <locale.h>
#include <iostream>

class NCursesGraphic : public IGraphic {
private:
	int		width, height;
	WINDOW	*gameWindow;
	bool	isInitialized;
	
public:
	NCursesGraphic() : width(0), height(0), gameWindow(nullptr), isInitialized(false) {}

	NCursesGraphic(const NCursesGraphic&) = delete;
	NCursesGraphic &operator=(const NCursesGraphic&) = delete;
	
	void init(int w, int h) override {
		setlocale(LC_ALL, "");
		width = w;
		height = h;
		
		std::cout << BBLU << "[NCurses] Initializing: " << width << "x" << height << RESET << std::endl;
		
		bool wasEnded = (isendwin() == TRUE);
		
		if (wasEnded || stdscr == nullptr) {
			std::cout << BBLU << "[NCurses] Starting fresh ncurses session" << RESET << std::endl;
			initscr();
		} else {
			std::cout << BBLU << "[NCurses] Reusing existing ncurses session" << RESET << std::endl;
			refresh();
		}
		
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		nodelay(stdscr, TRUE);
		curs_set(0);
		
		if (has_colors()) {
			start_color();
			init_pair(1, COLOR_GREEN, COLOR_BLACK);
			init_pair(2, COLOR_RED, COLOR_BLACK);
			init_pair(3, COLOR_BLACK, COLOR_BLACK);
		}
		
		// +2 for borders (1 left, 1 right, 1 top, 1 bottom)
		// Width doubled for square-ish cells
		gameWindow = newwin(
			height + 2,      // Height + borders
			(width * 2) + 3, // Width * 2 + left border + right border + 1
			1,               // Y position (row 1)
			1                // X position (col 1)
		);
		
		if (gameWindow) {
			keypad(gameWindow, TRUE);
			nodelay(gameWindow, TRUE);
		}

		bkgd(COLOR_PAIR(0));
		clear();
		refresh();
		
		isInitialized = true;
		//std::cout << BBLU << "[NCurses] Initialized successfully" << RESET << std::endl;
	}
	
	void render(const GameState& state) override {
		werase(gameWindow);
		
		wborder(gameWindow, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
		        ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
		
		wattron(gameWindow, COLOR_PAIR(3));
		for (int y = 1; y <= height; ++y) {
			for (int x = 1; x <= width; ++x) {
				mvwaddstr(gameWindow, y, x * 2, "  ");
			}
		}
		wattroff(gameWindow, COLOR_PAIR(3));
		
		// Draw snake
		wattron(gameWindow, COLOR_PAIR(1));
		for (int i = 0; i < state.snake->getLength(); ++i) {
			int y = state.snake->getSegments()[i].y + 1;
			int x = (state.snake->getSegments()[i].x * 2) + 1;
			
			if (i == 0) {
				mvwaddstr(gameWindow, y, x, "⬢ ");
			} else {
				mvwaddstr(gameWindow, y, x,
					(i == state.snake->getLength() - 1) ? "○ " :
					(i % 2 == 0) ? "✛ " : "✲ "
				);
			}
		}
		wattroff(gameWindow, COLOR_PAIR(1));
		
		// Draw food
		wattron(gameWindow, COLOR_PAIR(2));
		int foodX = (state.food->getPosition().x * 2) + 1;
		mvwaddstr(gameWindow, state.food->getPosition().y + 1, foodX, state.food->getFoodChar());
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
		if (!isInitialized) {
			return;
		}
		
		std::cout << BBLU << "[NCurses] Cleaning up..." << RESET << std::endl;
		
		// Delete subwindow first
		if (gameWindow) {
			werase(gameWindow);
			wnoutrefresh(gameWindow);
			doupdate();
			delwin(gameWindow);
			gameWindow = nullptr;
		}
		
		// Clear window without destroying/resetting it
		if (stdscr) {
			wclear(stdscr);
			wrefresh(stdscr);
		}
		
		// Reset terminal state
		fflush(stdout);
		
		isInitialized = false;
		std::cout << BBLU << "[NCurses] Destroyed (keeping ncurses session alive)" << RESET << std::endl;
	}
};

extern "C" IGraphic* createGraphic() {
	return new NCursesGraphic();
}

extern "C" void destroyGraphic(IGraphic* g) {
	delete g;
}
