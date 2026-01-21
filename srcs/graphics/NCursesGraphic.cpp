#include "../../incs/NCursesGraphic.hpp"

NCursesGraphic::NCursesGraphic() : width(0), height(0), gameWindow(nullptr), isInitialized(false) {}

NCursesGraphic::~NCursesGraphic() {
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
}

void NCursesGraphic::init(int w, int h) {
		setlocale(LC_ALL, "");
		width = w;
		height = h;
		
		bool wasEnded = (isendwin() == TRUE);
		
		if (wasEnded || stdscr == nullptr) {
			initscr();
		} else {
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
	}
	
	void NCursesGraphic::render(const GameState& state, float deltaTime) {
		(void)deltaTime;

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
	
	Input NCursesGraphic::pollInput() {
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
			case ' ':		return Input::Pause;
			default:        return Input::None;
		}
	}