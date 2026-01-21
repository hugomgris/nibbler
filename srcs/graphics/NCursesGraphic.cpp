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
	use_default_colors();
	
	if (can_change_color()) {
		// RGB to ncurses scale (0-1000): value = (RGB_value * 1000) / 255
		init_color(COLOR_RED, 705, 204, 227);    // RGB(180, 52, 58) - Dark red
		init_color(COLOR_BLUE, 275, 510, 706);   // RGB(70, 130, 180) - Steel blue
		init_color(COLOR_CYAN, 996, 290, 318);   // RGB(254, 74, 81) - Light red for food, called CYAN because I'm limited in my color count
	}
	
	init_pair(1, COLOR_BLUE, COLOR_BLACK);    // Snake (blue on black)
	init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Food (light red/cyan on black)
	init_pair(3, COLOR_BLACK, COLOR_BLACK);   // Background
	init_pair(4, COLOR_WHITE, COLOR_BLACK);   // UI text
}	
// +8 for quad borders (4 left, 4 right, 4 top, 4 bottom)
	// Width doubled for square-ish cells
	gameWindow = newwin(
		height + 8,      // Height + quad borders (4 top, 4 bottom)
		(width * 2) + 8, // Width * 2 + quad borders (4 left, 4 right)
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
	
	int win_height, win_width;
	getmaxyx(gameWindow, win_height, win_width);
	
	wattron(gameWindow, COLOR_PAIR(3));
	for (int y = 4; y <= height + 3; ++y) {
		for (int x = 0; x <= width; ++x) {
			mvwaddstr(gameWindow, y, (x * 2) + 4, "  ");
		}
	}
	wattroff(gameWindow, COLOR_PAIR(3));

	drawBorder();
	
	// Handle border offset (4 cells for quad border)
	wattron(gameWindow, COLOR_PAIR(1));
	for (int i = 0; i < state.snake->getLength(); ++i) {
		int y = state.snake->getSegments()[i].y + 4;
		int x = (state.snake->getSegments()[i].x * 2) + 4;
		
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
	
	wattron(gameWindow, COLOR_PAIR(2));
	int foodX = (state.food->getPosition().x * 2) + 4;
	//mvwaddstr(gameWindow, state.food->getPosition().y + 4, foodX, "▝▘");
	mvwaddstr(gameWindow, state.food->getPosition().y + 4, foodX, state.food->getFoodChar());
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

void NCursesGraphic::drawBorder() {
	int win_height, win_width;
	getmaxyx(gameWindow, win_height, win_width);

	wattron(gameWindow, COLOR_PAIR(4));

	// 4 layer border
	const char *layers[] = {"░", "▒", "▓", "█"};
	
	for (int layer = 0; layer < 4; layer++) {
		int offset = 3 - layer;
		const char *pattern = layers[layer];
		
		for (int x = offset; x < win_width - offset; x++) {
			mvwaddstr(gameWindow, offset, x, pattern);
		}
		
		for (int x = offset; x < win_width - offset; x++) {
			mvwaddstr(gameWindow, win_height - 1 - offset, x, pattern);
		}
		
		for (int y = offset + 1; y < win_height - 1 - offset; y++) {
			mvwaddstr(gameWindow, y, offset, pattern);
		}
		
		for (int y = offset + 1; y < win_height - 1 - offset; y++) {
			mvwaddstr(gameWindow, y, win_width - 1 - offset, pattern);
		}
	}

	wattroff(gameWindow, COLOR_PAIR(4));
}