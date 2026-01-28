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
		// Also: I'm reusing some colors (CYAN, GREEN) to cover for the possibility of not being able to generate new ones
		init_color(COLOR_RED, 705, 204, 227);		// Red
		init_color(COLOR_BLUE, 275, 510, 706);		// Blue
		init_color(COLOR_CYAN, 996, 290, 318);		// Light Red
		init_color(COLOR_GREEN, 300, 300, 300);		// Gray for ground 
	}
	
	init_pair(1, COLOR_BLUE, COLOR_BLACK);    // Snake (blue on black)
	init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Food (light red/cyan on black)
	init_pair(3, COLOR_BLACK, COLOR_BLACK);   // Background
	init_pair(4, COLOR_WHITE, COLOR_BLACK);   // UI text
	init_pair(5, COLOR_GREEN, COLOR_BLACK);   // Ground
	
	bkgd(COLOR_PAIR(0));
	clear();
	refresh();
}	
	// +8 for quad borders (4 left, 4 right, 4 top, 4 bottom)
	// Width doubled for square-ish cells
	gameWindow = newwin(
		height + 8,
		(width * 2) + 8,
		1,
		1
	);
	
	if (gameWindow) {
		keypad(gameWindow, TRUE);
		nodelay(gameWindow, TRUE);
	}
	
	generateGroundPattern();
	
	isInitialized = true;
}

void NCursesGraphic::render(const GameState& state, float deltaTime) {
	(void)deltaTime;

	werase(gameWindow);
	
	int win_height, win_width;
	getmaxyx(gameWindow, win_height, win_width);
	
	drawGround();

	drawBorder();
	
	drawSnake(state);
	
	drawFood(state);
	
	// Double buffer: stage stdscr first, then gameWindow
	wnoutrefresh(stdscr);
	wnoutrefresh(gameWindow);
	doupdate();
}

void NCursesGraphic::drawTitle(int win_height, int win_width)
{
	int lineCounter = 0;
	std::string filepath;

	if (win_width / 2 < 38)
	{
		// bbler
		filepath = "logos/ncurses_title_small_D.txt";

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(file, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(file, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(file, line))
			{
				wattron(gameWindow, COLOR_PAIR(4));
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(4));
				lineCounter++;
			}
			
		}

		// i base
		lineCounter = 0;
		filepath = "logos/ncurses_title_small_C.txt";
		std::ifstream fileC(filepath);
		if (!fileC.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileC, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileC, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileC, line))
			{
				wattron(gameWindow, COLOR_PAIR(1));
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(1));
				lineCounter++;
			}
			
		}

		// i dot
		filepath = "logos/ncurses_title_small_B.txt";

		std::ifstream fileB(filepath);
		if (!fileB.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileB, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileB, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileB, line))
			{
				wattron(gameWindow, COLOR_PAIR(2));
				mvwaddstr(gameWindow, anchorY + lineCounter - 3, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(2));
				lineCounter++;
			}
		}

		// n
		lineCounter = 0;
		filepath = "logos/ncurses_title_small_A.txt";

		std::ifstream fileA(filepath);
		if (!fileB.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileA, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileA, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileA, line))
			{
				wattron(gameWindow, COLOR_PAIR(4));
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(4));
				lineCounter++;
			}
		}
	}
	else {
		// bbler
		filepath = "logos/ncurses_title_big_D.txt";

		std::ifstream fileD(filepath);
		if (!fileD.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileD, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileD, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileD, line))
			{
				wattron(gameWindow, COLOR_PAIR(4) | A_BOLD);
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(4) | A_BOLD);
				lineCounter++;
			}
			
		}

		// i base
		lineCounter = 0;
		filepath = "logos/ncurses_title_big_C.txt";
		std::ifstream fileC(filepath);
		if (!fileC.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileC, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileC, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileC, line))
			{
				wattron(gameWindow, COLOR_PAIR(1));
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(1));
				lineCounter++;
			}
			
		}

		// i dot
		filepath = "logos/ncurses_title_big_B.txt";

		std::ifstream fileB(filepath);
		if (!fileB.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileB, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileB, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileB, line))
			{
				wattron(gameWindow, COLOR_PAIR(2));
				mvwaddstr(gameWindow, anchorY + lineCounter - 7, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(2));
				lineCounter++;
			}
		}

		// n
		lineCounter = 0;
		filepath = "logos/ncurses_title_big_A.txt";

		std::ifstream fileA(filepath);
		if (!fileB.is_open())
		{
			std::cerr << "Failed to fetch title file" << std::endl;
			return;
		}
		else 
		{
			std::string line;
			std::getline(fileA, line);
			int anchorX = (win_width - std::stoi(line)) / 2;
			std::getline(fileA, line);
			int anchorY = (win_height - std::stoi(line)) / 2;

			while (std::getline(fileA, line))
			{
				wattron(gameWindow, COLOR_PAIR(4));
				mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
				wattroff(gameWindow, COLOR_PAIR(4));
				lineCounter++;
			}
		}
	}
}

void NCursesGraphic::drawStartScreen(int win_height, int win_width) {
	int anchorY = ((win_width / 2) < 38) ? win_height / 2 : (win_height / 2) + 5;

	drawTitle(win_height, win_width);

	wattron(gameWindow, COLOR_PAIR(5));
	const char *instructions = "[ENTER]·······START";
	int instrY = anchorY + 2;
	mvwaddstr(gameWindow, instrY, 
			(win_width / 2) - 10, instructions);
	
	const char *controls = "[⬆⬇⬅➡]·········MOVE";
	int controlsY = instrY + 1;
	mvwaddstr(gameWindow, controlsY,
			(win_width / 2) - 10, controls);
	const char *libs = "[1/2/3]······TRAVEL";
	int libsY = controlsY + 1;
	mvwaddstr(gameWindow, libsY,
			(win_width / 2) - 10, libs);
	const char *quit = "[Q/ESC]········QUIT";
	int quitY = libsY + 1;
	mvwaddstr(gameWindow, quitY,
			(win_width / 2) - 10, quit);
	wattroff(gameWindow, COLOR_PAIR(5));
}

void NCursesGraphic::renderMenu(const GameState &state) {
	(void)state;
	
	werase(gameWindow);

	int win_height, win_width;
	getmaxyx(gameWindow, win_height, win_width);

	wattron(gameWindow, COLOR_PAIR(4));
	for (int x = 0; x < win_width - 1; x++)
	{
		for (int y = 0; y < win_height - 1; y++)
		{
			mvwaddch(gameWindow, y, x, ' ');
		}
	}
	wattroff(gameWindow, COLOR_PAIR(4));

	drawBorder();

	drawStartScreen(win_height, win_width);
	
	wnoutrefresh(stdscr);
	wnoutrefresh(gameWindow);
	doupdate();
}

void NCursesGraphic::drawGameOverTitle(int win_height, int win_width)
{
	int lineCounter = 0;
	std::string filepath;

	filepath = "logos/ncurses_gameover_big_B.txt";

	// OVER
	std::ifstream fileA(filepath);
	if (!fileA.is_open())
	{
		std::cerr << "Failed to fetch title file" << std::endl;
		return;
	}
	else 
	{
		std::string line;
		std::getline(fileA, line);
		int anchorX = (win_width - std::stoi(line)) / 2;
		std::getline(fileA, line);
		int anchorY = ((win_height - std::stoi(line)) / 2);

		while (std::getline(fileA, line))
		{
			wattron(gameWindow, COLOR_PAIR(2));
			mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
			wattroff(gameWindow, COLOR_PAIR(2));
			lineCounter++;
		}
		
	}

	// GAME
	filepath = "logos/ncurses_gameover_big_A.txt";
	lineCounter = 0;
	std::ifstream fileB(filepath);
	if (!fileB.is_open())
	{
		std::cerr << "Failed to fetch title file" << std::endl;
		return;
	}
	else 
	{
		std::string line;
		std::getline(fileB, line);
		int anchorX = (win_width - std::stoi(line)) / 2;
		std::getline(fileB, line);
		int anchorY = ((win_height - std::stoi(line)) / 2) - 4;

		while (std::getline(fileB, line))
		{
			wattron(gameWindow, COLOR_PAIR(1));
			mvwaddstr(gameWindow, anchorY + lineCounter, anchorX, line.c_str());
			wattroff(gameWindow, COLOR_PAIR(1));
			lineCounter++;
		}
		
	}
}

void NCursesGraphic::drawGameOverScreen(const GameState &state, int win_height, int win_width)
{
	(void)state;
	int anchorY = ((win_width / 2) < 38) ? win_height / 2 : (win_height / 2) + 5;

	drawGameOverTitle(win_height, win_width);

	wattron(gameWindow, COLOR_PAIR(5));
	//char *score = std::to_string(state.score).c_str();
	const char *instructions = "YOU ATE    APPLES";
	int instrY = anchorY + 4.5;
	mvwaddstr(gameWindow, instrY, 
			(win_width / 2) - 10, instructions);
	
	const char *controls = "[⬆⬇⬅➡]·········MOVE";
	int controlsY = instrY + 1;
	mvwaddstr(gameWindow, controlsY,
			(win_width / 2) - 10, controls);
	const char *libs = "[1/2/3]······TRAVEL";
	int libsY = controlsY + 1;
	mvwaddstr(gameWindow, libsY,
			(win_width / 2) - 10, libs);
	const char *quit = "[Q/ESC]········QUIT";
	int quitY = libsY + 1;
	mvwaddstr(gameWindow, quitY,
			(win_width / 2) - 10, quit);
	wattroff(gameWindow, COLOR_PAIR(5));
}

void NCursesGraphic::renderGameOver(const GameState &state)
{
	(void)state;
	werase(gameWindow);

	int win_height, win_width;
	getmaxyx(gameWindow, win_height, win_width);

	wattron(gameWindow, COLOR_PAIR(4));
	for (int x = 0; x < win_width - 1; x++)
	{
		for (int y = 0; y < win_height - 1; y++)
		{
			mvwaddch(gameWindow, y, x, ' ');
		}
	}
	wattroff(gameWindow, COLOR_PAIR(4));

	drawBorder();

	drawGameOverScreen(state, win_height, win_width);

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
		case 'Q':       return Input::Quit;
		case 27 :		return Input::Quit; // ESC key
		case '1':       return Input::SwitchLib1;
		case '2':       return Input::SwitchLib2;
		case '3':       return Input::SwitchLib3;
		case ' ':		return Input::Pause;
		case '\n':      return Input::Enter; // Enter key (also covers value 10)
		case KEY_ENTER: return Input::Enter; // Alternative Enter key
		default:        return Input::None;
	}
}

void NCursesGraphic::drawGround() {
	wattron(gameWindow, COLOR_PAIR(5) | A_DIM);
	for (int y = 4; y <= height + 3; ++y) {
		for (int x = 0; x <= width; ++x) {
			int screenX = (x * 2) + 4;
			char groundChar = groundPattern[y - 4][x];
			if (groundChar != ' ') {
				mvwaddch(gameWindow, y, screenX, groundChar);
				mvwaddch(gameWindow, y, screenX + 1, ' ');
			} else {
				mvwaddstr(gameWindow, y, screenX, "  ");
			}
		}
	}
	wattroff(gameWindow, COLOR_PAIR(5) | A_DIM);
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

void NCursesGraphic::drawSnake(const GameState &state) {
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
}

void NCursesGraphic::drawFood(const GameState &state) {
	wattron(gameWindow, COLOR_PAIR(2));
	int foodX = (state.food->getPosition().x * 2) + 4;
	mvwaddstr(gameWindow, state.food->getPosition().y + 4, foodX, state.food->getFoodChar());
	wattroff(gameWindow, COLOR_PAIR(2));
}

void NCursesGraphic::generateGroundPattern() {
	groundPattern.resize(height);
	for (int y = 0; y < height; ++y) {
		groundPattern[y].resize(width + 1);
	}
	
	const char groundChars[] = {
		'.', '.', '.', '.', '.',
		',', ',',
		'`', '`',
		';',
		':', ':',
		'-',
		' ', ' ', ' '
	};
	const int numChars = sizeof(groundChars) / sizeof(groundChars[0]);
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> charDist(0, numChars - 1);
	std::uniform_int_distribution<> densityDist(0, 100);
	
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x <= width; ++x) {
			if (densityDist(gen) < 75) {
				groundPattern[y][x] = groundChars[charDist(gen)];
			} else {
				groundPattern[y][x] = ' ';
			}
		}
	}
}