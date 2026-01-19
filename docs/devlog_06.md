# Nibbler - Devlog - 6

## Table of Contents
1. [Day Six Plan](#61-day-six-plan)
2. [Retroceder Nunca, Rendirse Jamás](#62-a-new-foe-has-appeared)
	- [External Ncurses Build](#621-external-ncurses-build-makefile)
	- [Persistent Session Strategy](#622-persistent-session-strategy-ncursesgraphiccpp)
	- [Smart Initialization](#623-smart-initialization)
	- [Exit handler](#624-exit-handler-maincpp)
	- [Overall Sum Up](#625-overall-sum-up)
3. [Food Restocking](#63-food-restocking)
4. [Randomizing the Starting Snake](#64-randomizing-the-starting-snake)
4. [Dimensional Ambitions](#65-dimensional-ambitions)
	- [Isometric Worldview 101](#isometric-worldview-101)
	- [Snake Cubism](#snake-cubism)


<br>
<br>
<br>

## 6.1 Day Six Plan
Monday again, the sadness persists, and so does the work. I'll pick it up just where I left it last Friday: **searching for an Ncurses substitute**. Besides that, some other things that could be done today:
- Refine the food spawning
- Start menu
- Refine the ending processsing
- Start experimenting with the `3D version` (cube-based) in Raylib

<br>
<br>
<br>

## 6.2 Retroceder Nunca, Rendirse Jamás
Or, as the original title of the Van Damme classic, the infamous Bruce Lee Ghost movie, *No Retreat, No Surrender*, but the spanish title is arguably way cooler. After the seg fault disaster which ended last Friday's work session, I came up with an idea: **What if instead of transitioning into a new library, I just switch into an external build of Ncurses and avoid the system one?**. And lo and behold: it worked.

### Understanding the Problem
The root cause of the segfault was becoming clearer with each test: **system NCurses maintains global state that gets corrupted during runtime library switching**. The flow looked like this:

1. Library is loaded (`dlopen`)
2. NCurses initialized (`initscr`)
3. NCurses cleaned up (`endwin`)
4. Library unloaded (`dlclose`)
5. **Repeat steps 1-4** ← State corruption and segfault!

The issue was particularly visible when switching back and forth between NCurses and any other library. SDL2 ↔ Raylib worked perfectly fine, but NCurses ↔ anything would crash after a couple of switches. This pointed to something specific about how NCurses handles its internal state at the system level.

<br>

### The Solution: A Three-Pronged Approach

The fix required changes in three key areas: **the build system, the NCurses graphics implementation, and the main program cleanup**. Let's break them down:

#### 6.2.1 External NCurses Build (Makefile)

Instead of using the system NCurses (which maintains problematic global state), the build now:
- Downloads NCurses 6.4 from source
- Compiles it locally in `libs/ncurses/`, just like it was already doing for SDL2 and Raylib
- Links all binaries against the local build
- Full control over library state!

The Makefile modifications were straightforward, following the same pattern already established for SDL2 and Raylib:

```makefile
# -=-=-=-=-    EXTERNAL LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

NCURSES_DIR     := $(LIB_DIR)/ncurses
NCURSES_URL     := https://invisible-mirror.net/archives/ncurses/ncurses-6.4.tar.gz
NCURSES_EXISTS  := $(wildcard $(NCURSES_DIR)/lib/libncursesw.so)

NCURSES_CFLAGS  := $(LIB_CFLAGS) -I$(NCURSES_DIR)/include -I$(NCURSES_DIR)/include/ncursesw
NCURSES_LDFLAGS := -L$(NCURSES_DIR)/lib -lncursesw -Wl,-rpath,$(NCURSES_DIR)/lib
```

The build target downloads, configures, and compiles NCurses with the necessary flags:

```makefile
ifndef NCURSES_EXISTS
	@echo "$(YELLOW)NCurses not found. Downloading and building...$(DEF_COLOR)"
	@mkdir -p $(LIB_DIR)
	@cd $(LIB_DIR) && \
		curl -L -o ncurses.tar.gz $(NCURSES_URL) && \
		tar xzf ncurses.tar.gz && \
		mv ncurses-6.4 ncurses-src && \
		cd ncurses-src && \
		./configure --prefix=$(PWD)/$(NCURSES_DIR) \
			--enable-widec \
			--with-shared \
			--without-debug \
			--enable-pc-files \
			--with-pkg-config-libdir=$(PWD)/$(NCURSES_DIR)/lib/pkgconfig && \
		make -j4 && \
		make install && \
		cd .. && \
		rm -rf ncurses-src ncurses.tar.gz
	@echo "$(GREEN)NCurses built successfully$(DEF_COLOR)"
endif
```

Key configuration flags:
- `--enable-widec`: Enables wide character support (Unicode)
- `--with-shared`: Builds shared libraries
- `--prefix`: Sets installation to our local directory

The main program also needed to link against the local NCurses for the exit handler:

```makefile
LDFLAGS := -ldl -L$(PWD)/libs/ncurses/lib -lncursesw -Wl,-rpath,$(PWD)/libs/ncurses/lib
```

<br>

#### 6.2.2 Persistent Session Strategy (NCursesGraphic.cpp)

This was the key insight: **instead of destroying the NCurses session on every library switch, keep it alive**. The destructor now only cleans up windows, not the entire session:

```cpp
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
    
    // Clear main screen but DON'T call endwin()
    // This keeps ncurses alive between library switches
    if (stdscr) {
        wclear(stdscr);
        wrefresh(stdscr);
    }
    
    fflush(stdout);
    isInitialized = false;
    std::cout << BBLU << "[NCurses] Destroyed (keeping ncurses session alive)" << RESET << std::endl;
}
```

The critical change: **no `endwin()` call in the destructor**. This keeps the NCurses session persistent across library switches.

<br>

#### 6.2.3 Smart Initialization

The `init()` function needed to be smart enough to detect whether NCurses needs a fresh start or can reuse an existing session:

```cpp
void init(int w, int h) override {
    setlocale(LC_ALL, "");
    width = w;
    height = h;
    
    std::cout << BBLU << "[NCurses] Initializing: " << width << "x" << height << RESET << std::endl;
    
    // Check if ncurses was previously ended (isendwin returns TRUE if endwin was called)
    bool wasEnded = (isendwin() == TRUE);
    
    if (wasEnded || stdscr == nullptr) {
        // Initialize ncurses if it was ended or never started
        std::cout << BBLU << "[NCurses] Starting fresh ncurses session" << RESET << std::endl;
        initscr();
    } else {
        std::cout << BBLU << "[NCurses] Reusing existing ncurses session" << RESET << std::endl;
        refresh();
    }
    
    // ... rest of initialization
}
```

**Important note**: I initially got the `isendwin()` logic backwards! After testing, I discovered:
- `isendwin()` returns `0` (FALSE) when NCurses is active OR never initialized
- `isendwin()` returns `1` (TRUE) when `endwin()` has been called

This was a crucial debugging moment that cost me a bit of time but taught me to always verify library behavior with small test programs.

<br>

#### 6.2.4 Exit Handler (main.cpp)

Finally, since we're no longer calling `endwin()` during library switches, we need to ensure it gets called when the program actually exits. Enter `std::atexit()`:

```cpp
#include <ncurses.h>

// Cleanup handler for ncurses when program exits
void cleanupNCurses() {
    if (isendwin() == FALSE) {
        endwin();
        std::cout << BYEL << "[Main] Called endwin() on exit" << RESET << std::endl;
    }
}

int main(int argc, char **argv) {
    // Register cleanup handler
    std::atexit(cleanupNCurses);
    
    // ... rest of main
}
```

This guarantees that the terminal is properly reset even if the program exits abnormally.

<br>

### 6.2.5 Overall Sum-up

The new flow with a local NCurses build and persistent session:

1. Library is loaded (`dlopen`)
2. NCurses initialized ONCE (`initscr`)
3. Windows cleaned up, **session stays alive**
4. Library unloaded (`dlclose`)
5. **Repeat 1,3,4** → NCurses session persists!
6. Final cleanup on program exit (`endwin`)

By keeping the NCurses session alive and using a locally compiled version, the execution can:
- Avoid corrupting the global state
- Maintain full control over the library lifecycle
- Enable seamless switching between all three libraries

<br>
<br>
<br>

## 6.3 Food Restocking
With no more segfaults, let's tackle other pending subjects, like enhancing the repositioning of the food dot when nibbled. The main issue with the current implementation is that because it's just a brute, random index-based repositioning, there are cases in which the dot gets repositioned in an occupied cell (right now, a snake-occupied cell; in the future there might be other space-occuping stuff). To refine the repositioning process, two main ways come to mind:
- A retry based approach → Just check if the repositioning is targetting an occupied space, if so re-replace (funcional but not optimal, and once again baby Jesus would cry)
- Map the available space in the game arena so that the repositioning never targets an occupied space (refined, elegant, baby Jesus smile worthy).

So let's map. The only thing that's needed is to reserve the exact space in memory that will be needed, then go through all the cells in the game arena and check if there's a snake chunk in it. If there's not, it's available. After that, get the position from the available list, not the overall arena list. And we're golden:

```cpp
bool Food::replaceInFreeSpace(GameState *gameState)
{
	std::vector<Vec2> snakeSegments;
	for (int i = 0; i < gameState->snake->getLength(); i++) {
		snakeSegments.push_back(gameState->snake->getSegments()[i]);
	}

	std::vector<Vec2> availableCells;
	availableCells.reserve(_hLimit * _vLimit - snakeSegments.size());

	for (int y = 0; y < _vLimit; y++) {
		for (int x = 0; x < _hLimit; x++)
		{
			Vec2 candidate = {x, y};

			bool occupied = false;
			for (const auto &segment : snakeSegments) {
				if (segment.x == candidate.x && segment.y == candidate.y)
				{
					occupied = true;
					break;
				}
			}

			if (!occupied) {
				availableCells.push_back(candidate);
			}
		}
	}

	if (availableCells.empty())
	{
		std::cout << "No available cells! aka you Won, bb" << std::endl;
		return false;
	}

	int randomIndex = Utils::getRandomInt(availableCells.size());
	_position = availableCells[randomIndex];
	_foodChar = Utils::getFoodChar(Utils::getRandomInt(5));

	return true;
}
```

<br>
<br>
<br>

## 6.4 Randomizing the Starting Snake
Easy enough, but needs to be done. We'll just change the snake's constructor to pick a random int based direction and head position, clamp the latter so that it is fairly centered in the game space (so that it doesn't, for example, spawn a left bound snake too close to the right wall and give time for player reaction), and be happy:
```cpp
Snake::Snake(int width, int height): _length(4) {
	switch (Utils::getRandomInt(3))
	{
		case 0:
			_direction = UP;
			break;
		case 1:
			_direction = DOWN;
			break;
		case 2:
			_direction = LEFT;
			break;
		case 3:
			_direction = RIGHT;
			break;
	}

	Vec2 headPosition = { Utils::getRandomRangeInt(8, width - 8), Utils::getRandomRangeInt(8, height - 8) };

	switch (_direction) {
		case UP:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y + 1 };
			_segments[2] = { headPosition.x, headPosition.y + 2 };
			_segments[3] = { headPosition.x, headPosition.y + 3 };
			break;

		case DOWN:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x, headPosition.y - 1 };
			_segments[2] = { headPosition.x, headPosition.y - 2 };
			_segments[3] = { headPosition.x, headPosition.y - 3 };
			break;

		case LEFT:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x + 1, headPosition.y };
			_segments[2] = { headPosition.x + 2, headPosition.y };
			_segments[3] = { headPosition.x + 3, headPosition.y };
			break;

		case RIGHT:
			_segments[0] = headPosition;
			_segments[1] = { headPosition.x - 1, headPosition.y };
			_segments[2] = { headPosition.x - 2, headPosition.y };
			_segments[3] = { headPosition.x - 3, headPosition.y };
			break;
	}
}
```

<br>
<br>
<br>

## 6.5 Dimensional Ambitions
Transitioning into a 3D render is a straightforward process given that we're drawing a *snake* game. The 2D versions (ASCII based and colored-cell based) are drawn following a square pattern that just needs a height value to achieve tridimensionality. In other words, we just need to think in cubes, instead of cells. With that in mind, two main things need to be done to reach our 3D ambitions:
- An isometric camera setup
- A cube-based build and rendering

Raylib was chosen precisely because achieven these two things should be quite simple, so let's try it.

### Isometric Worldview 101
First, we need a `Camera3D` with a position calculation `x` and `z` based on the game session's grid witdth and height, all while taking into consideration the cube size. Then, the camera's direction needs to be tilted, set to `orthographic` and... Good to go. Basically, your run-of-the-mill standard isometric view setup, so let's take the opportunity to break down how to build this POV regardless of the tool used to do so:

#### I. Define the World Space
The bounds of the scene need to be calculated to correctly set up the relations between the rendered construction and the camera. What we need first is **the center point towards which the camera will be positioned**:
- Center point → `(sceneWidth/2, 0, sceneHeight_2)`

#### II. Position the Camera
A standard isometric view needs **specific values** for both the camera's `elevation` and `rotation`:
- `Elevation angle` → Typically `30º` to `45º` above the ground
- `Rotation angle` → `45º` around the Y-axis (this is what gives that *diamond look* that we're looking for)

This is all then processed through some simple trigonometry:

```
distance = max(sceneWidth, sceneHeight) * unit_size * scale_factor
camera_x = center_x + distance * cos(rotation) * cos(elevation)
camera_y = center_y + distance * sin(elevation)
camera_z = center_z + distance * sin(rotation) * cos(elevation)
```
> In the specific case of this *snake*, i.e. the `Raylib` implementation, I use the cube size as `unit_size`.

#### III. Set Camera Properties
After the base calculations, the camera's properties that need to be defined (how they're defined depends on the tool/library) are:
- `Position`: where the camera sits in 3D space
- `Target`: What point the camera looks at (usually, the scene center)
- `Up vector`: Which direction is "up" (usually, it's [0, 1, 0])
- `Projection`: The type of visualization wanted (usually, tools include both `ORTHOGRAPHIC` and `PERSPECTIVE`)

#### IV. Adjust Field of View / Ortho Size
The last step is to finely tune the *optics*, which will depend on the `Projection`:
- `Orthographic`: Control the "zoom" via `ortho width/height`
- `Perspective`: Control the "FOV" via angle value (the usual is `45º`)

#### V. The Implementation Itself:
All if this translates in this small camera setup function inside the `RaylibGraphic` class:
```cpp
void setupCamera() {
		float centerX = (gridWidth * cubeSize) / 2.0f;
		float centerZ = (gridHeight * cubeSize) / 2.0f;
		
		// Calculate diagonal distance to ensure entire grid fits
		float diagonal = sqrtf(gridWidth * gridWidth + gridHeight * gridHeight) * cubeSize;
		float distance = diagonal * 1.2f;  // 20% padding
		
		// Standard isometric angles: 35.264° elevation, 45° rotation
		float elevation = 35.264f * DEG2RAD;  // Classic isometric angle
		float rotation = 45.0f * DEG2RAD;
		
		camera.position = (Vector3){ 
			centerX + distance * cosf(rotation) * cosf(elevation),
			distance * sinf(elevation),
			centerZ + distance * sinf(rotation) * cosf(elevation)
		};
		
		camera.target = (Vector3){ centerX, cubeSize * 3, centerZ }; // "* 3" is there to adjust the centering of the scene
		camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
		camera.fovy = 60.0f;
		camera.projection = CAMERA_ORTHOGRAPHIC;
	}
```

<br>

### Snake Cubism
The rest of our 3D ascension is just a redefinition of the drawing and managing functions to work with cubes instead of 2D cells. Pretty self-explanatory, to be honest, so let's just make a compilation of the new stuff. 

For now, I'll make functions for drawing **floor, walls, snake and food**:
```cpp
void RaylibGraphic::drawGroundPlane() {
	for (int z = 0; z < gridHeight; z++) {
		for (int x = 0; x < gridWidth; x++) {
			Color squareColor = ((x + z) % 2 == 0) ? lightSquare : darkSquare;
			
			Vector3 position = {
				x * cubeSize,
				0.0f,
				z * cubeSize
			};
			
			DrawCube(position, cubeSize, cubeSize, cubeSize, squareColor);
		}
	}
}
```
```cpp
void RaylibGraphic::drawWalls() {
	// Stack of 3 cubes for walls (temporarily)
	for (int level = 0; level < 3; level++) {
		float yPos = (level) * cubeSize;
		
		for (int x = -1; x <= gridWidth; x++) {
			// Top wall
			Vector3 topPos = { x * cubeSize, yPos, -cubeSize };
			DrawCube(topPos, cubeSize, cubeSize, cubeSize * 2, wallColor);
			
			// Bottom wall
			Vector3 bottomPos = { x * cubeSize, yPos, gridHeight * cubeSize };
			DrawCube(bottomPos, cubeSize, cubeSize, cubeSize, wallColorFade);
		}
		
		for (int z = 0; z < gridHeight; z++) {
			// Left wall
			Vector3 leftPos = { -cubeSize, yPos, z * cubeSize };
			DrawCube(leftPos, cubeSize, cubeSize, cubeSize, wallColor);
			
			// Right wall
			Vector3 rightPos = { gridWidth * cubeSize, yPos, z * cubeSize };
			DrawCube(rightPos, cubeSize, cubeSize, cubeSize, wallColor);
		}
	}
}
```
```cpp
void RaylibGraphic::drawSnake(const Snake* snake) {
	// Draw snake at level 1 (one cube above ground)
	float yPos = cubeSize;  // Center of level 1
	
	for (int i = 0; i < snake->getLength(); i++) {
		const Vec2& segment = snake->getSegments()[i];
		
		Vector3 position = {
			segment.x * cubeSize,
			yPos,
			segment.y * cubeSize
		};
		
		if (i == 0) {
			DrawCube(position, cubeSize, cubeSize, cubeSize, headColor);
		} else {
			// Body pieces set up to 80% of cube size
			position.y = position.y * .8f;
			(i % 2 == 0) ?
				DrawCube(position, cubeSize *.8f, cubeSize *.8f, cubeSize *.8f, BodyColor_1) :
				DrawCube(position, cubeSize *.8f, cubeSize *.8f, cubeSize *.8f, BodyColor_2) ;
		}
	}
}
```
```cpp
void RaylibGraphic::drawFood(const Food* food) {
	float yPos = cubeSize;
	
	Vec2 foodPos = food->getPosition();
	Vector3 position = {
		foodPos.x * cubeSize,
		yPos,
		foodPos.y * cubeSize
	};
	
	// Pulsing effect for food
	float pulse = 1.0f + sinf(GetTime() * 3.0f) * 0.1f;
	
	DrawCube(position, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, cubeSize * 0.7f * pulse, foodColor);
}
```
> *The food cube has a pulsating effect (time based) to make it look fancy*

The rest is just going through Raylib's `3DMode` pipeline, via `BeginMode3D()` and `EndMode3D()` inside `render()`, and watch how our little ascii snake reaches the relm of tridimensionality:
```cpp
void RaylibGraphic::render(const GameState& state){
	BeginDrawing();
	ClearBackground(RAYWHITE);
	
	BeginMode3D(camera);
	
	// Draw in order: ground -> walls -> snake -> food
	drawGroundPlane();
	//drawWalls();
	drawSnake(state.snake);
	drawFood(state.food);
	
	// Optional: Draw grid lines for debugging
	// DrawGrid(gridWidth, cubeSize);
	
	EndMode3D();
	
	// 2D HUD overlay
	DrawText("Press 1/2/3 to switch libraries", 10, 10, 20, DARKGRAY);
	DrawText("Arrow keys to move, Q/ESC to quit", 10, 35, 20, DARKGRAY);
	DrawFPS(screenWidth - 95, 10);
	
	EndDrawing();
}
```
<br>

---
> I think this is enough for today. Tomorrow I'll try to work on the entry and exit points (with some menuing and whatnot).