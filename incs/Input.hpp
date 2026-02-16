#pragma once

/*
Each library will map to this enum:
	-Ncurses keys
	-SDL events
	-Raylib input
*/

enum class Input {
	None,
	Switch2D,
	Switch3D,
	Up,			// for compatibility, will be deleted once every library is updated to handle multiple snakes
	Down,		// for compatibility, will be deleted once every library is updated to handle multiple snakes
	Left,		// for compatibility, will be deleted once every library is updated to handle multiple snakes
	Right,		// for compatibility, will be deleted once every library is updated to handle multiple snakes
	Up_A,		// Player A Up
	Down_A,		// Player A Down
	Left_A,		// Player A Left
	Right_A,	// Player A Right
	Up_B,		// Player B Up
	Down_B,		// Player B Down
	Left_B,		// Player B Left
	Right_B,	// Player B Right
	Quit,
	Pause,
	Enter,
	ToggleFS
};