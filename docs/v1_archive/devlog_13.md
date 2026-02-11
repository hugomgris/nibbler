# Nibbler - Devlog - 13

## Table of Contents
1. [Day Seventeen Plan](#131-day-seventeen-plan)
2. [Testing, Testng, One, Two, Three](#132-testing-testng-one-two-three)
	- [Makefiling Our Way into Googletest's Heart](#makefiling-our-way-into-googletests-heart)
	- [Lights, Camera, Github Actions!](#lights-camera-github-actions)
	- [Googling Ment Something Different When I Was Younger (or The Google Way of Testing Stuff)](#googling-ment-something-different-when-i-was-younger-or-the-google-way-of-testing-stuff)


<br>
<br>

## 13.1 Day Seventeen Plan
Today is all about automated tests. Well, I hope it is going to be *more* than about those, but we'll start from there, for which we'll need to set up:
- github actions
- makefile build pipeline and test-testing files to set up a `gtest` (Google) environment
- write a bunch of tests

If after all of this there's more time to today's journey, I'll get into... **checks notes*... I don't know, building the foundations for a multisnake mode or something, don't ask me, what do i know.

<br>
<br>

## 13.2 Testing, Testng, One, Two, Three
I have some experience setting up github actions with automated test pipelines based on different tools. For example, I've worked with `xUnit` in `C#`/`dotnet` environments and `criterion` for `C` programs, and after some research I decided to take the opportunity to learn about `gtest` tied to `C++`. All of these tools, as I understand them at least, give me the perfect environment to both make **assertion based tests** that can be automated a different points during development and integration (aka, we're getting into `CI/CL` from this point on), and the objective today is to **build a pipeline that auto-tests any attempt to push or merge along the project's tree**. So let's get into it!

As this is a somewhat tangly implementation, which needs substantial additions to `Makefile`, test boilerplating and the `yml` configuration files for the `github actions`, this is also a good opportunity to lay down the whole process, step by step. We'll do it in this order:
1. Makefile setup for `googletest`
2. Github actions setup (`yml` files)
3. How to write tests, with some examples (snake tests)

### Makefiling Our Way into Googletest's Heart
I must say something, before going on: I hate Makefile. I love it *conceptually*, but I'd rather eat glass than writing one of these files. Fortunately, at this point in my life, I have so many makefiles in my backpack that there's always one that works as a template, but injecting automated tests in them is something that I've only started doing since the last months of last year. So let's break down how to do it in plain human.

Adding a `googletest` implementation to my current mafile requires the following:
- A test directory target
- The test suit URL from which we'll get the source code to implement `googletest`
- The target library (`.a` file) that the test suite builds
- In this case, the `main` related sub-library that the suite also builds
- The lib's build process includes

On top of this, to manage the test files and their setup, we need to set up directory for the test files (my written tests), their object and dependencies hidden folders and the independent binary that will be used to run the tests.

> For this last part, I use `wildcard` because I don't want to bother myself with having to write every test source file manually every time I write one. While wildcarding is sometimes discouraged for production code (where explicit dependency tracking matters), for test discovery it's the pragmatic choice. If someone has anything against this decision, all I can resort to is asking for forgiveness.

Translating all of this to the cursed tongue of Makefile, and sprinkling some flags on top of everything, we get:
```Makefile
# -=-=-=-=-    GOOGLE TEST -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

GTEST_DIR		:= $(LIB_DIR)/googletest
GTEST_REPO		:= https://github.com/google/googletest.git
GTEST_LIB		:= $(GTEST_DIR)/build/lib/libgtest.a
GTEST_MAIN_LIB	:= $(GTEST_DIR)/build/lib/libgtest_main.a
GTEST_INCLUDES	:= -I$(GTEST_DIR)/googletest/include

# -=-=-=-=-    TEST CONFIGURATION -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

TEST_DIR		:= tests
TEST_OBJDIR		:= .test_obj
TEST_DEPDIR		:= .test_dep
TEST_BINARY		:= run_tests

# Test source files (we'll add these)
TEST_SRCS		:= $(wildcard $(TEST_DIR)/unit/*.cpp) \
					$(wildcard $(TEST_DIR)/integration/*.cpp)
TEST_OBJS		:= $(patsubst $(TEST_DIR)/%.cpp,$(TEST_OBJDIR)/%.o,$(TEST_SRCS))
TEST_DEPS		:= $(patsubst $(TEST_DIR)/%.cpp,$(TEST_DEPDIR)/%.d,$(TEST_SRCS))

# Need to compile main program objects for testing (exclude main.cpp)
TESTABLE_SRCS	:= $(filter-out $(SRCDIR)/main.cpp, $(SRCS))
TESTABLE_OBJS	:= $(addprefix $(OBJDIR)/, $(notdir $(TESTABLE_SRCS:.cpp=.o)))

TEST_CFLAGS		:= $(CFLAGS) $(GTEST_INCLUDES)
TEST_LDFLAGS	:= $(LDFLAGS) -lpthread
```

Once this done, the `check` and `target` sections related to test need to be added. Nothing extremely arcane here, although it freaking looks like it, let's be real. Check looks inside the test directory to see if a gtest build is in place, goes into building process if there's not. Target runs the written test through the testing pipeline. All together, this is achieved by adding the following:
```Makefile
check_gtest:
	@if [ ! -f "$(GTEST_LIB)" ]; then \
		echo "$(YELLOW)Google Test not found. Cloning and building...$(DEF_COLOR)"; \
		mkdir -p $(LIB_DIR); \
		git clone --depth 1 --branch v1.14.0 $(GTEST_REPO) $(GTEST_DIR); \
		cd $(GTEST_DIR) && mkdir -p build && cd build && \
		cmake -DCMAKE_CXX_STANDARD=20 .. && \
		make -j4; \
		echo "$(GREEN)Google Test built successfully$(DEF_COLOR)"; \
	else \
		echo "$(GREEN)Google Test already built$(DEF_COLOR)"; \
	fi
```
```Makefile
# -=-=-=-=-    TEST TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

test: check_gtest $(TEST_BINARY)
	@echo "$(CYAN)Running tests...$(DEF_COLOR)"
	./$(TEST_BINARY)

$(TEST_BINARY): $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB)
	@echo "$(YELLOW)Linking test binary...$(DEF_COLOR)"
	$(CC) -o $@ $(TESTABLE_OBJS) $(TEST_OBJS) $(GTEST_LIB) $(GTEST_MAIN_LIB) $(TEST_LDFLAGS)
	@echo "$(GREEN)Test binary created: $(TEST_BINARY)$(DEF_COLOR)"

# Compile test object files
$(TEST_OBJDIR)/%.o: $(TEST_DIR)/%.cpp | $(TEST_OBJDIR) $(TEST_DEPDIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(TEST_DEPDIR)/$*.d)
	$(CC) $(TEST_CFLAGS) -MMD -MF $(TEST_DEPDIR)/$*.d -c $< -o $@
	@echo "$(BLUE)Compiled test: $<$(DEF_COLOR)"

# Create test directories
$(TEST_OBJDIR):
	@mkdir -p $(TEST_OBJDIR)/unit $(TEST_OBJDIR)/integration

$(TEST_DEPDIR):
	@mkdir -p $(TEST_DEPDIR)/unit $(TEST_DEPDIR)/integration
```

Now, all we have to do is write test (something pretty enjoyable for me during the, let's say, first hour of coding them, not so much after that >_>).

#### A Quick Note on SDL2 Headers
One tricky issue I encountered during CI setup: when building SDL2 from source (as the Makefile does), headers are in `libs/SDL2/include/SDL.h`. However, system-installed SDL2 uses `<SDL2/SDL.h>`. 

Therefore, I had to standardized the includes  (`#include <SDL.h>`) in the code, and the Makefile flags now correctly handle the path resolution:
```cpp
// From this:
#include <SDL.h>

// to this:
#include <SDL2/SDL.h>
```

This makes the code compatible with both custom builds and system packages, which is essential for CI environments that use apt-installed libraries.

<br>

### Lights, Camera, Github Actions!

Now that we have the Makefile pipeline ready, we need to automate it via GitHub Actions. This requires creating workflow files in `.github/workflows/`.

#### ci.yml - Full Build and Test Pipeline
```yaml
name: CI/CD Pipeline

on:
  push:
    branches: [ main, dev ]
  pull_request:
    branches: [ main, dev ]

jobs:
  build-and-test:
    runs-on: ubuntu-latest
    
    steps:
    - name: Checkout code
      uses: actions/checkout@v4
      with:
        submodules: recursive
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          build-essential \
          cmake \
          libx11-dev \
          libxrandr-dev \
          libxinerama-dev \
          libxcursor-dev \
          libxi-dev \
          libgl1-mesa-dev \
          libglu1-mesa-dev \
          libsdl2-dev \
          libsdl2-ttf-dev \
          libsdl2-mixer-dev \
          libncurses-dev \
          pkg-config \
          git
    
    - name: Cache libraries
      uses: actions/cache@v4
      with:
        path: |
          libs/SDL2
          libs/SDL_ttf
          libs/SDL_mixer
          libs/raylib
          libs/ncurses
          libs/googletest
        key: ${{ runner.os }}-libs-${{ hashFiles('Makefile') }}-v2
        restore-keys: |
          ${{ runner.os }}-libs-
    
    - name: Build libraries
      run: |
        make check_libs
        make check_gtest
    
    - name: Build project
      run: make all
    
    - name: Run tests
      run: make test
    
    - name: Upload test results
      if: always()
      uses: actions/upload-artifact@v4
      with:
        name: test-results
        path: |
          run_tests
          *.log
```

This workflow:
- Triggers on pushes to `main`/`dev` branches and on pull requests
- Installs necessary system dependencies (including SDL2, NCurses dev packages)
- Caches built libraries for faster subsequent runs
- Builds everything and runs tests
- Fails the build if any test fails
- Uploads test results as artifacts for debugging

#### test.yml - Fast Test-Only Pipeline
For rapid feedback on code changes, I wanted a lighter workflow that only runs tests:
```yaml
name: Tests

on:
  push:
    paths:
      - 'tests/**'
      - 'srcs/**'
      - 'incs/**'
      - 'Makefile'
  pull_request:
    paths:
      - 'tests/**'
      - 'srcs/**'
      - 'incs/**'
      - 'Makefile'

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v4
    
    - name: Install test dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake git
    
    - name: Cache Google Test
      uses: actions/cache@v4
      with:
        path: libs/googletest
        key: ${{ runner.os }}-gtest-${{ hashFiles('Makefile') }}
    
    - name: Build Google Test
      run: make check_gtest
    
    - name: Build and run tests
      run: make test
```

This runs only when test or source files change, providing faster feedback during development. It's perfect for catching test failures early without the overhead of building all graphics libraries (which at this point in the project take a lot of time).

<br>

### Googling Ment Something Different When I Was Younger (or The Google Way of Testing Stuff)
All assertion based tests, when trying to automate them via some suit and strictly following my limited experience and knowledge about them, work the same:
- You set up a running environment (i.e., what the specifically needs from the whole program build to work)
- You research how the assertions are specifically done in the suite of choice
- You write small, controlled test with a very controlled focus.

For example, a snake related test in this *nibbler* written for `gtest` can look like this:
```cpp
#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"

class SnakeTest : public ::testing::Test {
	protected:
		void SetUp() override {

		}

		void TearDown() override {

		}
};

TEST_F(SnakeTest, SnakeInitialization) {
	Snake snake(20, 20);

	EXPECT_EQ(snake.getLength(), 4);
}

TEST_F(SnakeTest, SnakeMovement) {
	Snake snake(20, 20);
	Direction dir = snake.getDirection();
	Vec2 initialPos = snake.getSegments()[0];

	snake.move();
	Vec2 newPos = snake.getSegments()[0];

	if (dir == Direction::Left || dir == Direction::Right)
		EXPECT_NE(initialPos.x, newPos.x);
	else
		EXPECT_NE(initialPos.y, newPos.y);
}
```

There's:
- Necessary includes, both for `gtest` and just the header files that are needed for the tests included in thhe file
- A test class with minimal implementation (this varies depending on the suite)
- The test functions themselves
	- How to write them and the assertion function names are, obviously, dependent on the suite too, but conceptually they are always pretty alike

And we can now spend the rest of our day (hope not) writing tests!!!!

#### What Should Be Test?

Good tests for this *Nibbler* project should focus, I think, on:
- **Pure logic**: Snake movement, collision detection, growth mechanics
- **Data structures**: Vec2 operations, segment management, food positioning
- **State transitions**: Menu → Playing → GameOver flow
- **Edge cases**: Boundary collisions, self-collision, minimum grid size
- **Game rules**: Score tracking, speed progression, food spawning

The goal here is to ensure the *game logic* is bulletproof, so that when bugs appear, it is first and foremost clear that they're in the presentation layer, not the core mechanics. This means that regarding the presentation layer, which covers graphics rendering, library initialization, and visual elements will be relegated to a sort of *visual* testing, as automating them in a logic-based way would be quite complicated, if possible at all. Besides this, user input related stuff is also best tested either manually or via **integration tests**, which I'll tackle later on.

<br>

I'll back with some more examples of what I code today in relation to tests...

... And here I am. I've built unit snake test and a couple of integration tests to check the interaction between `snake`, `GameState`, and `GameManager`, all passing :D

> Unit tests:
```cpp
#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"

class SnakeTest : public ::testing::Test {
	protected:
		void SetUp() override {

		}

		void TearDown() override {

		}
};

TEST_F(SnakeTest, SnakeInitialization) {
	Snake snake(20, 20);

	EXPECT_EQ(snake.getLength(), 4);
}

TEST_F(SnakeTest, SnakeMovement) {
	Snake snake(20, 20);
	Direction dir = snake.getDirection();
	Vec2 initialPos = snake.getSegments()[0];

	snake.move();
	Vec2 newPos = snake.getSegments()[0];

	if (dir == Direction::Left || dir == Direction::Right)
		EXPECT_NE(initialPos.x, newPos.x);
	else
		EXPECT_NE(initialPos.y, newPos.y);
}

TEST_F(SnakeTest, SnakeGrowth) {
	Snake snake(20, 20);
	int initialLength = snake.getLength();

	snake.grow();
	int newLength = snake.getLength();

	EXPECT_FALSE(initialLength == newLength);
	EXPECT_EQ(initialLength, 4);
	EXPECT_EQ(newLength, 5);

	for (int i = 0; i < 10; i++)
		snake.grow();

	EXPECT_EQ(snake.getLength(), 15);
}
```
<br>

> Integragion tests:
```cpp
#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"
#include "../../incs/GameManager.hpp"

class SnakeCollisionTest : public ::testing::Test {
	protected:
		void SetUp() override {

		}

		void TearDown() override {

		}
};

TEST_F(SnakeCollisionTest, SelfCollision) {
	GameState state =  TestHelpers::createBasicGameState(20, 20);
	GameManager manager(&state);
	Snake &snake = state.snake;

	if (snake.getDirection() != Direction::Right)
		snake.changeDirection(Direction::Left);
	else {
		snake.changeDirection(Direction::Up);
		manager.update();
		snake.changeDirection(Direction::Left);
	}

	manager.update();
	snake.grow();
	manager.update();
	snake.grow();
	manager.update();
	snake.grow();
	manager.update();
	snake.grow();
	manager.update();
	snake.grow();

	// Check if already colliding
	manager.update();

	if (snake.getDirection() != Direction::Right)
		snake.changeDirection(Direction::Left);
	manager.update();
	manager.update();
	snake.changeDirection(Direction::Down);
	manager.update();
	manager.update();
	snake.changeDirection(Direction::Right);
	manager.update();
	manager.update();
	snake.changeDirection(Direction::Up);
	manager.update();
	manager.update();

	EXPECT_FALSE(state.isRunning);
}

TEST_F(SnakeCollisionTest, WallCollision) {
	GameState state =  TestHelpers::createBasicGameState(20, 20);
	GameManager manager(&state);
	Snake &snake = state.snake;
	std::cout << "Game running: " << state.isRunning << std::endl;

	if (snake.getDirection() == Direction::Right || snake.getDirection() == Direction::Left) {
		snake.changeDirection(Direction::Up);
		manager.update();
	} else if (snake.getDirection() == Direction::Down) {
		snake.changeDirection(Direction::Left);
		manager.update();
		snake.changeDirection(Direction::Up);
		manager.update();
	}

	// Snake surely moving upwards until reaching boundary -> y = 0
	while (snake.getSegments()[0].y > 0)
	{
		manager.update();
	}
	
	// GameManager detects the out-of-bounds position 
	manager.update();

	EXPECT_FALSE(state.isRunning);
	EXPECT_EQ(snake.getSegments()[0].y, -1);
}
```

<br>

Well, back to write more tests...

...Had to refine some stuff when creating the test classes, so that every piece needed by them is handled by smart pointers in the class, and via reference in the test function themselves. Nothing too complicated. Anyway, here's the food related integration tests:
```cpp
#include <gtest/gtest.h>
#include "../../incs/Snake.hpp"
#include "../../incs/DataStructs.hpp"
#include "../fixtures/TestHelpers.hpp"
#include "../../incs/GameManager.hpp"

class FoodInteractionTest : public ::testing::Test {
	protected:
		std::unique_ptr<Snake> snake;
		std::unique_ptr<Food> food;
		std::unique_ptr<GameState> state;
		std::unique_ptr<GameManager> manager;
		
		void SetUp() override {
			snake = std::make_unique<Snake>(20, 20);
			food = std::make_unique<Food>(Vec2{10, 10}, 20, 20);
			state = std::make_unique<GameState>(GameState{
				20, 20,
				*snake, *food,
				false, true, false,
				GameStateType::Playing,
				0, nullptr
			});
			manager = std::make_unique<GameManager>(state.get());
		}
		
		void TearDown() override {}
};

TEST_F(FoodInteractionTest, FoodPositioning) {
	Snake &snakeRef = state->snake;
	Food &foodRef = state->food;

	// moving the snake a bit

	for (int i = 3; i > 0; i--)
		snakeRef.move();

	// the key thing here is the repositioning of the food, which triggers when eating happens
	// after a repositioning, the food should always be in bounds and never be in a snake-occupied spot
	for (int i = 5; i > 0; i--) {
		foodRef.replaceInFreeSpace(state.get());

		int x = foodRef.getPosition().x;
		int y = foodRef.getPosition().y;

		EXPECT_GE(x, 0);
		EXPECT_LT(x, 20);
		EXPECT_GE(y, 0);
		EXPECT_LT(y, 20);

		const Vec2 *segments = snakeRef.getSegments();
		int snakeLength = snakeRef.getLength();

		while (--snakeLength > 0)
		{
			EXPECT_FALSE(segments[snakeLength].x == x && segments[snakeLength].y == y);
		}
	}
}

TEST_F(FoodInteractionTest, EatingTrigger) {
	Snake &snakeRef = state->snake;

	// Normalize direction to go upwards
	if (snakeRef.getDirection() == Direction::Right || snakeRef.getDirection() == Direction::Left) {
		snakeRef.changeDirection(Direction::Up);
		manager->update();
	} else if (snakeRef.getDirection() == Direction::Down) {
		snakeRef.changeDirection(Direction::Left);
		manager->update();
		snakeRef.changeDirection(Direction::Up);
		manager->update();
	}
	
	//Force a food position ABOVE the snake head and to its RIGHT
	while (state->food.getPosition().x <= snakeRef.getSegments()[0].x || state->food.getPosition().y >= snakeRef.getSegments()[0].y)
		state->food.replaceInFreeSpace(state.get());

	Vec2 foodPosition = state->food.getPosition();

	EXPECT_TRUE(state->food.getPosition().x > snakeRef.getSegments()[0].x && state->food.getPosition().y < snakeRef.getSegments()[0].y);

	// Move the head of the snake to collide with the food
	int i = state->snake.getSegments()[0].y - state->food.getPosition().y;
	std::cout << i << std::endl;

	while (i > 0) {
		manager->update();
		i--;
	}

	snakeRef.changeDirection(Direction::Right);
	i = state->food.getPosition().x - state->snake.getSegments()[0].x;

	// midway check -> food should still be in the same position
	EXPECT_TRUE(foodPosition.x == state->food.getPosition().x && foodPosition.y == state->food.getPosition().y);

	while (i > 0) {
		manager->update();
		i--;
	}
	
	Vec2 newFoodPosition = state->food.getPosition();

	EXPECT_TRUE(foodPosition.x != newFoodPosition.x && foodPosition.y != newFoodPosition.y);
}
```

I added some more tests, and now I'm going home, considering that I've done enough tests for the day.

Current extra stuff status:
Regarding bonus stuff:
- [x] 3D graphics (and overall deep-thought graphic design across platforms)
- [ ] Configurable settings: modularity and user customization
- [ ] Enhance gameplay (implement one of the recolected ideas)
- [x] Sounds
- [x] Automated tests
- [x] Multiplayer
- [ ] AI opponent
- [ ] Performance metrics and debug mode
- [ ] Custom maps
- [ ] Level editor
- [ ] Replay system
- [ ] Turbo-refined abstraction
- [x] Documentation