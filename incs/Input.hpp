#pragma once

/*
Each library will map to this enum:
	-Ncurses keys
	-SDL events
	-Raylib input
*/

enum class Input {
	None,
	SwitchLib1,
	SwitchLib2,
	SwitchLib3,
	Up,
	Down,
	Left,
	Right,
	Quit,
	Pause,
};