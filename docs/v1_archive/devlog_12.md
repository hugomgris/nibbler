# Nibbler - Devlog - 12

## Table of Contents
1. [Day Sixteen Plan](#121-day-sixteen-plan)
2. [A Very Fun and Thorough and Exciting Combing of the Subject's Requirements](#122-a-very-fun-and-thorough-and-exiciting-combing-of-the-subjects-requirements)
3. [Finishing moves](#133-finishing-moves)
4. [Interfacing 2: Electric Boogaloo](#134-interfacing-2-electric-boogaloo)

<br>
<br>

## 12.1 Day Sixteen Plan
Who would have guessed it, look at as, here again, in a brand new log. Today's plans are going to mainly revolve around general assessments in different aspects:
- General state of the SUBJECT project: what's done, what's to be done
- General state of the PERSONAL project: recollection of ideas, restatement of goals
- Cleanup, norm adjustments, that kind of boring stuff
- Probably throw myself down the pitt* of graphics stuff once more

> *Great show, recommended

<br>
<br>

## 12.2 A Very Fun and Thorough and Exiciting Combing of the Subject's Requirements
Let's just get to the point(s). Re-reading the subject, this is what I need to take care of:
- [ ] Enhance argument parsing: clamp the introduced values, only numeric
- [ ] Check that every class: 1) doesn't define stuff in the header file, 2) is canonical

Regarding bonus stuff:
- [x] 3D graphics (and overall deep-thought graphic design across platforms)
- [ ] Configurable settings: modularity and user customization
- [ ] Enhance gameplay (implement one of the recolected ideas)
- [ ] Sounds
- [ ] Automated tests
- [ ] Multiplayer
- [ ] AI opponent
- [ ] Performance metrics and debug mode
- [ ] Custom maps
- [ ] Level editor
- [ ] Replay system
- [ ] Turbo-refined abstraction
- [x] Documentation

Obviously, just need 5 of these regarding the turning in of the project, but all of these features are interesting, challenging and overall welcomed, so we'll see. Just in case, let's list what they would entrail:

1. Configurable Settings System
	- Config file (JSON/INI) for game parameters (colors, speeds, keybindings)
	- Hot-reload settings without restart
	- Menu options

2. Custom maps && level editor
	- Allow text-based map input (similar as an `FDF` or a `solong`)
	- New branch in the program that displays a blank canvas and can be interacted with to build a custom map
		- Obviously save that map in a text format

3. Performance metrics and debug mode
	- FPS counter, frame time graphs
	- Memory usage tracking

4. Multiplayer
	- Render, poll and manage two snakes, with a new set of controls

5. AI opponent
	- Pathfinding algorithm (A* or Dijkstra)
	- Difficulty levels

6. Replay system
	- Record and playback gameplay sessions
	- Save the movements and positions in a file (JSON?)

7. Turbo abstraction:
	- Same features work identically across libraries
	- At least first level of every implementation is interfaced and homogeneous across libraries

7. Sounds
	- Dynamic management of sounds
	- Possible new `.so` library

8. Automated tests
	- Implement `gtest` based github actions


<br>
<br>

## 13.3 Finishing Moves
It's time to end the basic project, which should be fast an simple, just the parsing enhancement and the class related revisions. I also have to fix some scale twitch stuff in `Raylib` when going into menus (*sigh*).

Parsing just need the most basic approach, just clamp the min and max values and search for anything that is not a number in all the arguments. If found, reject:
```cpp
bool parseArguments(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		std::string str(argv[i]);
		if (str.find_first_not_of("0123456789") != std::string::npos) {
			std::cerr << "error: bad argument {" << argv[i] << "}: only numeric arguments accepted" << std::endl;
			return false;
		}
	}

	return true;
}
```
As for the cannonical form requirements, this was just a boring pass to make sure that every class had it's necessary constructors, overloads and etceteras, as well as taking some function definitions I had in headers to their respective `cpp` files.

The twitching stuff in the `Raylib` menus was also a nothingburger, I just had the camera's FOV update too late in the menu pipeline. Rellocating it to the beginning of said pipeline fix the issue.

And thus, the basic project ins D O N E.

<br>
<br>

## 13.4 Interfacing 2: Electric Boogaloo
Let's start adding extra stuff, and let's do it from the audio side of things. We'll do this by interfacing the audio related stuff, so that we can (as we are asked to) manage everything in the library side of stuff. The implementation itself is going to be done by my team partner, but I'll take care of `IAudio`:
```cpp
class IAudio {
	public:
		virtual ~IAudio() = default;

		// init
		virtual void init() = 0;
		virtual void cleanup() = 0;

		// sound management
		virtual bool loadSound(const std::string &id, const std::string &filepath) = 0;
		virtual void unloadSound(const std::string &id) = 0;

		// playback
		virtual void playSound(const std::string &id) = 0;
		virtual void playMusic(const std::string &id, bool loop = true);
		virtual void stopMusic() = 0;
		virtual void pauseMusic() = 0;
		virtual void resumeMusic() = 0;

		// Volume control -> normalized between 0.0f and 1.0f
		virtual void setSoundVolume(float volume) = 0;
		virtual void setMusicVolume(float volume) = 0;
		virtual void setMasterVolume(float volume) = 0;

		// Helpers
		virtual bool isMusicPlaying() const = 0;
};

extern "C" {
	IAudio *createAudio();
	void destroyAudio(IAudio*);
}
```

Some refactoring (renaming, really) need to be done in the graphics side of the `LibraryManager` to correctly divide the graphics and the audio pipelines. Now, a couple of things need to be dealt with:
1) the loading of the library in main
2) the setup of the audio lib accesibility (I'm thinking about a pointer to `IAudio` in `GameState`, accessible from the `GameManager`)
3) the most basic boilerplate setup for my parter
4) the Makefile additions to fetch and prepare the audio library of choice (thinking about `SDL_mix`)

And... All of those done. Don't need to get into detail, I think, as this is pretty boring stuff: renaming this, refactoring that, extending this other thing, placeholdering something over there... The main point is that right now, **the sound pipeline is in place**, and a fake sound play call is correctly done from the function that handles the eating of the food. From here, it's all in my partner's hands.

Tomorrow I'll get into automated tests!
