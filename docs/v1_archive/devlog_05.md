# Nibbler - Devlog - 5

## Table of Contents
1. [Day Five Plan](#51-day-five-plan)
2. [Fridays Are Lazy, But I Am Not](#52-fridays-are-lazy-but-i-am-not)
	- [Buffering Inputs](#buffering-inputs)
	- [My Seg Is Faulting](#my-seg-is-faulting)

<br>
<br>
<br>

## 5.1 Day Five Plan
Well, today is Friday, and although Fridays are not exactly the most productive days there are, I left myself a list of possible tasks prepared in yesterday's log to avoid the dread. I don't really hope to get too much done today, but here it goes:
- Buffering inputs
- Refine the food spawning
- Start menu
- Refine the ending processsing
- Start experimenting with the `3D version` (cube-based) in Raylib

<br>
<br>
<br>

## 5.2 Fridays Are Lazy, But I Am Not
It's been 3 or 4 hours since I wrote the title of this section, went to undergo review in some other project I did a couple of days ago and to review some peer's work, and came back. Ironic, as this is now going to look as I am lazy, but at 15:28 of a Friday, the day is looking grim production wise. I have atmost one hour left of work inside me, though, so let's try to put it to good use.

<br>

### Buffering Inputs
Because the game has a relatively fast tick update, players can press keys faster than the game updates and/or have some quickly sent inputs combine and result in unwanted situations, like the snake inverting it's direction 180 degrees and colliding with itself and commiting a weird act of suicide. **Storing inputs in a queue and processing one input per game tick** should fix this situation, so let's try that.

Buffering inputs means, broadly, that instead of having an immediate handling of the movement inputs, there's going to be an intermediate step: **there will be an Input queue inside the `GameManager` to wich these inputs will be pushed**. The size of the queue will be limited to 3 to avoid excesive buffering, and the processing of the queue will be delegated to a new function, called from the manager's `update`, tying the processing to the ticking. In other words, the movement inputs will take effect every frame call, same as the movement calculation of the snake itself. Some minor changes in `Main`, in its loop, and in the `GameManager` class:
```cpp
while (state.isRunning) {
		gameManager.calculateDeltaTime(&lastTime, &accumulator);
		
		Input input = gfxLib.get()->pollInput();
		
		if (input == Input::Quit) {
			std::cout << BYEL << "\nBYEBYEBYEBYE" << RESET << std::endl;
			state.isRunning = false;
			break;
		}
		
		if (input >= Input::SwitchLib1 && input <= Input::SwitchLib3) {
			int newLib = (int)input - 1;
			
			if (newLib != currentLib) {
				std::cout << BMAG << "\nSwitching from lib " << (currentLib + 1) 
						<< " to lib " << (newLib + 1) << RESET << std::endl;
				
				gfxLib.unload();
				
				if (!gfxLib.load(libs[newLib])) {
					std::cerr << BRED << "Failed to load new library!" << RESET << std::endl;
					return 1;
				}
				
				gfxLib.get()->init(width, height);
				currentLib = newLib;
			}
		}
		
		gameManager.bufferInput(input);
		
		while (accumulator >= FRAME_TIME) {
			gameManager.update();
			frameCount++;
			accumulator -= FRAME_TIME;
		}
		
		gfxLib.get()->render(state);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
```
```cppp
class GameManager {
	private:
		GameState	*_state;
		std::queue<Input> inputBuffer;
		static const size_t MAX_BUFFER_SIZE = 3;

		using time = std::chrono::time_point<std::chrono::high_resolution_clock>;

		void processNextInput();

	public:
		GameManager(GameState *state);

		GameManager(const GameManager &other) = delete;
		GameManager &operator=(const GameManager &other) = delete;

		~GameManager() = default;

		void update();

		void calculateDeltaTime(time *lastTime, double* accumulator);
		void bufferInput(Input input);

		void checkHeadFoodCollision();
		bool checkGameOverCollision();
};
```
```cpp
void GameManager::bufferInput(Input input) {
	if (input >= Input::Up && input <= Input::Right) {
		if (inputBuffer.size() < MAX_BUFFER_SIZE) {
			inputBuffer.push(input);
		}
	}
}

void GameManager::processNextInput() {
	if (!inputBuffer.empty()) {
		Input input = inputBuffer.front();
		inputBuffer.pop();
		
		switch (input) {
			case Input::Up:
				_state->snake->changeDirection(UP);
				break;
			case Input::Down:
				_state->snake->changeDirection(DOWN);
				break;
			case Input::Left:
				_state->snake->changeDirection(LEFT);
				break;
			case Input::Right:
				_state->snake->changeDirection(RIGHT);
				break;
			default:
				break;
		}
	}
}
```

> Only the movement related inputs are buffered. Quit and library switching inputs are still processed on the spot.

<br>

### My Seg Is Faulting
While testing the input buffering, i came across a bug: **if I switch a couple of times between `Ncurses` an any other library, I go into segfault**. This doesn't happen between `SDL2` and `Raylib`, so there's a clue. I'll now go hunt this little crash...

...And apprently, it's happening in the render call at the bottom of the main loop, `gfxLib.get()->render(state);`. Something must be wrong with how I either destroy and/or init something in Ncurses, and something tells me that it's going to be related to the subwindow...

... Let's break down the switching flow between Ncurses and, for example, Raylib:
1. Ncurses calls `initscr()` and creates windows
2. When switching to Raylib, Ncurses destructor calls `endwin()`, cleaning up terminal state
3. When switching back to Ncurses, `init()` is called, creating a new `gameWindow`
4. `render()` calls `doupdate`, which tries to access the internal Ncurses state that... **was destroyed**

So, although the pointer is valid, there are internal screen buffers that might be corrupted or dangling, because when there's an attempt to read from them from `doupdate()`... segfault.

The solution? **Fully reinitialize Ncurses when switching back to it**? Nope, that didn't do it. **Try to control the global states stored in the system by Ncurses?** Again, nope. **Keep Ncurses loaded in memory even when rendering through other library**? Nah, still gefaulting.

So, at this point, I think there's an evident conclussion lingering over all this process: **it seems like `Ncurses` is not the best library for this dynamic switching project**.

Which means that I'm going to need to find another library as a substitute.

Which, itself, means that it's time to end the week. I'll back on monday and transition into a new library.
