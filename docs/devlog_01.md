# Nibbler - Devlog - 1

## Table of Contents
1. [Understanding the Project](#11---understanding-the-project)
    1. [Project Overview](#project-overview)
    2. [Testlib example](#testlib-example)
    3. [Core concepts](#core-concepts)
    4. [Target architecture](#target-architecture)
    5. [Day-one](#day-one)
    6. [Common traps](#common-traps)
2. [Unswering Some Questions](#12---answering-some-questions)
    1. [What are C ABI (Application Binary Interface) boundaries?](#1--what-are-c-abi-application-binary-interface-boundaries)
    2. [What does "sharing STL containers across boundaries" mean](#2--what-does-sharing-stl-containers-across-boundaries-mean)
    3. [The meanings of `SDL`, `SFML`, `Ncurses`](#3--the-meanings-of-sdl-sfml-ncurses)
    4. [Build System Understanding (Makefile & Dependencies)](#4--build-system-understanding-makefile--dependencies)
3. [Day One Implemenations](#13---day-one-implementations)

<br>
<br>
<br>

# 1.1 - Understanding the Project
Starting a new project always feels a little bit daunting, specially when the main aim is to learn new tools and make a reasonably quick implementation of them in order to put them inside your backpack with some confidence. That's why this time, right before embarking on a journey to the land of Dynamic Libraries and Runtime Switching, I'm going to thoroughly log each step along the way.

Think of this document as a *what-I-am-trying-to-do* log more than a *what-has-been-done* account, although I guess it will become the latter after the project is, well, done.

## Project overview
At its core, Nibble is **not** a graphics project and **not** a Snake project (although we will make it be those too, *evil laugh). **This is a pugin architecture project**. It aims to prove that the team can:
- Write a **game engine** (logic, rules, state, loop)
- Completely **decouple rendering and input**
- Load **multiple implementations** of rendering/input **at runtime**
- Swap those implementations *while the game is running*
- Respect **C ABI boundaries** while still using C++ internally

The snake game is just a test case to validate the architecture, as if the design is correct:
- *Snake* can be replaced with any other game (e.g., *Tetris*) without touching the graphic libs
- Add a 4th graphica backend without touching the game logic

## Testlib Example
A basic starting point for a dynamic library build can be comprised of three files: `main.cpp`, `plugin.cpp` and `plugin.h`. A minimal approach (minus graphics) that demonstrates **the only safe pattern** for C++ dynamic libs must take into consideration that:
- The **main program owns the game**
- The **dynamic library owns its internal classes**
- The **only ABI-visible functions are `extern "C"`**
- Objects are:
	- **created inside the `.so`**
	- **destroyed inside the `.so`**
- Communication happens **only through pure virtual interfaces**

The code snippets below cover these considerations by establishing this core equivalencies:
| Example Concept    | Nibbler equivalent              |
| ------------------ | ------------------------------- |
| `host` executable  | `nibbler` main program          |
| `libplugin.so`     | One graphical/input library     |
| `Iplugin`          | Our `IgraphicLib` / `IRenderer` |
| `create_plugin`    | `create_renderer()`             |
| `destroy_plugin`   | `destroy_renderer()`            |
| `HostCallbacks`    | Game → renderer communication   |
| `dlopen/dlsym`     | Mandatory in Nibbler            |


### main.cpp
```C++
#include "plugin.h"
#include <dlfcn.h>
#include <iostream>
#include <string>

class HostCallbacks : public IHostCallbacks {
public:
    void log(const char* msg) override {
        std::cout << "[host] " << msg << "\n";
    }

    int add(int a, int b) override {
        return a + b;
    }
};

int main() {
    void* handle = dlopen("./libplugin.so", RTLD_NOW);
    if (!handle) {
        std::cerr << "dlopen error: " << dlerror() << "\n";
        return 1;
    }

    using create_fn_t  = IPlugin* (*)(IHostCallbacks*);
    using destroy_fn_t = void (*)(IPlugin*);

    dlerror();

    auto create_plugin  = (create_fn_t)dlsym(handle, "create_plugin");
    const char* e1 = dlerror();
    if (e1) {
        std::cerr << "dlsym(create_plugin) error: " << e1 << "\n";
        dlclose(handle);
        return 1;
    }

    auto destroy_plugin = (destroy_fn_t)dlsym(handle, "destroy_plugin");
    const char* e2 = dlerror();
    if (e2) {
        std::cerr << "dlsym(destroy_plugin) error: " << e2 << "\n";
        dlclose(handle);
        return 1;
    }

    HostCallbacks host;
    IPlugin* plugin = create_plugin(&host);

    plugin->start();
    plugin->do_work(10);

    destroy_plugin(plugin);
    dlclose(handle);
    return 0;
}
```

### plugin.cpp
```C++
#include "plugin.h"
#include <string>

class PluginImpl : public IPlugin {
public:
    explicit PluginImpl(IHostCallbacks* host) : host_(host) {}

    void start() override {
        host_->log("[plugin] start() llamado");
        int r = host_->add(2, 40);
        host_->log((std::string("[plugin] host->add(2,40) = ") + std::to_string(r)).c_str());
    }

    void do_work(int x) override {
        host_->log("[plugin] do_work() llamado");
        int r = host_->add(x, 1);
        host_->log((std::string("[plugin] add(x,1) = ") + std::to_string(r)).c_str());
    }

private:
    IHostCallbacks* host_;
};

extern "C" IPlugin* create_plugin(IHostCallbacks* host) {
    return new PluginImpl(host);
}

extern "C" void destroy_plugin(IPlugin* p) {
    delete p;
}
```

### plugin.h
```C++
#pragma once
#include <cstddef>

class IHostCallbacks {
public:
    virtual ~IHostCallbacks() = default;
    virtual void log(const char* msg) = 0;
    virtual int  add(int a, int b) = 0;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual void start() = 0;
    virtual void do_work(int x) = 0;
};

extern "C" {
    IPlugin* create_plugin(IHostCallbacks* host);
    void     destroy_plugin(IPlugin* p);
}
```

## Core Concepts
Nibbler asks to harness conceptual knowledge in several areas: `Dynamic Loading`, `ABI vs C++`, `Inversion of Control (IoC)`, `Interface Uniformity Across Libraries`.

### Dynamic Loading (mandatory)
Dynamic loading of libraries requires full understanding and handling of:
- `dlopen`
- `dlsym`
- `dlclose`
- `dlerror`

Above these (or below, whatever), one must understand:
- What happens if a symbol is missing
- What happens if a library fails to load
- Why function names must be unmangled (`extern "C"`)

### ABI vs C++ (it's a trap!)
Tha main idea here is that **C++ is NOT ABI-stable across compilation units**, therefore:
- The project **cannot**:
	- Export C++ classes directly
	- Share STL containers across boundaries
- The project **must**:
	- Export only C-style entry points
	- Use abstract interfaces (`class IFoo { virtual ...}`)
	- Allocate & free objects on the same side

### Inversion of Control (IoC)
A classic **engine → platform separation** must be achieved, storing the main loop in the program's own `main` and delegating to the renderer both to call drawing actions and to ask for user input.

The graphics library must **never**:
- Decide game rules
- Move the snake
- Spawn food
- Run its own game loop

Instead:
- The **main loop lives in `main`**
- Each frame:
	- main updates game state
	- main asks renderer to draw
	- main asks renderer for input

### Interface Uniformity Across Libraries
The main program must not know:
- Which library is SDL
- Which library is SFML
- Which library is Ncurses

They  must all **implement the same interface**. This means that if there's a need for something like the snippet below, the prooject is already an embarassing failure:
```C++
if (lib == SDL) do_x();
```

## Target Architecture
A clean model should be based on **Two main logic sides with `pure virtual C++ interfaces` in between**. On one side, the **`game engine`**, on the other, the **`draw calls and input polls`**. Here's a diagram of this approach:

```
 			   ┌──────────────────────┐
               │        main          │
               │    (game engine)     │
               │                      │
               │ - Snake logic        │
               │ - Game loop          │
               │ - Collision rules    │
               │ - Food spawning      │
               └──────────┬───────────┘
                          │
			 ┌────────────┴─────────────┐
             │Pure virtual C++ interface│
			 └────────────┬─────────────┘
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ lib_graphic1 │  │ lib_graphic2 │  │ lib_graphic3 │
│   (SDL)      │  │ (SFML)       │  │ (Ncurses)    │
│              │  │              │  │              │
│ draw()       │  │ draw()       │  │ draw()       │
│ pollInput()  │  │ pollInput()  │  │ pollInput()  │
└──────────────┘  └──────────────┘  └──────────────┘
```

Inside this structure, some **suggested minimal interfaces are**:
```C++
enum class Input {
    None,
    Left,
    Right,
    Quit,
    Switch1,
    Switch2,
    Switch3
};

class IGraphic {
public:
    virtual ~IGraphic() = default;
    virtual void init(int width, int height) = 0;
    virtual void render(const GameState&) = 0;
    virtual Input pollInput() = 0;
};
```
And **exported functions**:
```C++
extern "C" {
    IGraphic* create_graphic();
    void destroy_graphic(IGraphic*);
}
```

## Day One
For a comprehensive Day-One attack plan, the main goal should be to **have a running executable that loads a dummy dynamic library and switches it at runtime**. Obviously, no game yet. The plan itself can be split into the following steps:

### Step 1 - Skeleton Repository
- `src/`
	- `main.cpp`
	- `Game/` (emtpy on day one)
	- `Interfaces/`
- `lib/`
	- `lib1/`
	- `lib2/`
	- `lib3/`
- `Makefile`

### Step 2 - Define interfaces (no logic yet)
- `Igraphic.hpp`
- `Input.hpp`
- `GameState.hpp` (even empty)

**No STL containers across boundary yet**

### Step 3 - Dummy graphic library
Implement one `.so` file that:
- Prints to stdout instead of drawing
- Retrns fake input
- Compiles and loads correctly

Basically, replicate the example `testlib` from above, but renamed and simplified.

### Step 4 - dlopen pipeline
In `main`:
- Parse arguments
- `dlopen` first library
- `dlsym(create/destroy)`
- Call `init`
- Run a fake loop (e.g., 100 iterations)
- `dlclose`

### Step 5 - Runtime switching
Add:
- Pressing `1/2/3`:
	- `destroy_graphic`
	- `dlclose`
	- `dlopen` new `.so`
	- `create_graphic`
	- Continue grame loop.

When this works, the basic architecture will be done.


## Common Traps
Trying to prevent every possible pitfall in an evaluation would be insane, but here's some of them that the project should take into consideration from the starting point:
- Including SDL/SFML headers in `main`
- Game loop inside the library
- STL containers crossing `.so` boundary
- `new` in main, `delete` in lib (or viceversa)
- `using namespace std`
- Forgetting cannonical form
- One library having "extra features" main knows about.

<br>
<br>
<br>

# 1.2 - Answering some questions
Before building more concrete stuff (i.e., tackling that Day One list), here are some questions that I personally feel the need to answer. 

## 1- What are C ABI (Application Binary Interface) boundaries?
A **C ABI boundary** is the point where **binary compatibility matters**, *not source compatibility*. When one uses the following snippet, one is no longer "linking C++ code", but **calling raw symbols** by name at runtime:
```C++
dlopen + dlsym
```
The **C ABI** only ABI that can be reliably depended on across:
- Compilers
- Compiler versions
- Optimization flags
- Shared objecs

C++ breaks here because it has all these features that are **not standarized at the binary level**:
- name mangling
- multiple inheritance layouts
- vtables
- exception models
- STL implementations

An example:
```C++
IPlugin *create_plugin();
```
The *actual* symbol name might be:
```
_Z13create_pluginv
```
or something else, depending on compiler, platform and flags. So `dlsym("create_plugin)` **will not find it**.

#### What `extern "C"` does
An extern "C" call such as:
```C++
extern "C" IPlugin* create_plugin();
```
means:
- No name mangling
- C calling convention
- Predictable symbol name

This is why, as told by the project's subject, **entry points must be C, internals can be C++**. In Nibbler tearms, this has the following repercusions:

**Allowed across the boundary**
- `exter "C"` functions
- pointers to abstract base classes
- POD types (ints, enums, structs without constructors)

***Forbidden across the boundary**
- Concrete C++ classes
- Templates
- STL containers
- Exceptions
- `std::string`

## 2- What does "sharing STL containers across boundaries" mean
Firstly, **what is the meaning of "sharing"** (beyond caring, that is)?
If **either** of these happens, STL sharing is happening across boundaries:
```C++
// BAD
virtual void render(std::vector<Point> const &snake) = 0;

// BAD
virtual std::string getName() = 0;

// BAD
extern "C" void draw(std::map<int, int> *data);
```

Reasons:
- `std::vector`, `std::string`, `std::map`:
	- have internal memory layouts
	- depend on allocator
	- depend on libstdc++ version
- main and `.so` may not agree on those details

Even if *it works in a given machine*, it can:
- crash during defense
- break on another OS dump
- break under valgrind

#### Safe alternative
Instead of writing something like:
```C++
std::vector<Segment>
```
Write:
```C++
struct Segment {
	int x;
	int y;
}

struct SnakeView{
	Segment	const *segments;
	size_t	count;
}
```
This is:
- POD (Plain Old Data)
- ABI-stable
- ownership-explicit
Which means:
- main owns all data
- lib only *reads* it
- lib never allocates or frees it

A useful rule of thumb:
> **If it has a constructor, destructor, or template parameters — do not cross the boundary.**

## 3- The meanings of `SDL`, `SFML`, `Ncurses`
While the implementation of these **types of libraries** is not explicitly asked, it is suggested to "Use *different graphical backends* that demonstrate abstraction", and after some research it seems like most Nibbler projects use the following due to them being **very different paradigms**, **easy to justify abstraction**, **well-known to evaluators**:
- Ncurses (terminal)
- SDL2
- SFMl

Due to personal interests, and seeing that, also after some research, implementing `OpenGL` would be "overkill" and `Vulkan` would be a "self-harm open display", the (modern but safe) choice of libraries for this Nibbler will be:
- [Ncurses](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [Raylib](https://www.raylib.com/)
- [SDL2](https://www.libsdl.org/)

## 4- Build System Understanding (Makefile & Dependencies)
Careful consideration must mediate when building the dependency pipelines inside the Makefile, as the building process must **build the `.so` files that will be used by the compiled program while not commiting any external library resources**, all while making sure that the project **builds on a fresh machine**. Some options to manage this are:

#### Option 1 - System packages
- Use `apt`, `brew` or `dnf`
- Provide:
	- `README`
	- pr `setup.sh`

This is acceptable, but the fact this **might need `sudo`** is a strong argument agains it.

#### Option 2 - Local build via script
- `scripts/install_deps.sh`
- Downloads
- Builds
- Installs locally

More work, but more robust (and `sudo` avoiding).

### Makefile role
Whatever the chosen option is, the `Makefile` should:
- Build `.so` with `-fPIC -shared`
- Link against system-installed libs
- Not download anything itself

Example:
```make
libncurses.so:
	c++ -fPIC -shared ncurses.cpp -lncurses -o libncurses.so
```

**An importante nuance**:
> **The main executable must `NOT` link against graphic libs**
If `ldd nibbler` shows SDL, SFMl, Raylib, etc → **`TURBOFAIL`**. Only the `.so` files should link them.

<br>
<br>
<br>

# 1.3 - Day One Implementations
With the foundational understanding of what Nibbler asks and some of the main questions risen by the subject answered, Day One implementations will start with:
1. Building the skelleton of the repository
2. Defining the starting points of the initial interfaces

### The Skelleton of the Repository
Nothing fancy here, just the good old `srcs` / `libs`/ `incs`/ setup, with a `docs` subdirectory to store pre/peri/post production documentation. Each subdirectory in `libs/` is already named after the choice of libraries, and a `testlib` subdirectory is included for the Day One test build (i.e., will be removed once the test build is done and the project moves on to real library management).

```
.
├── docs
│   └── Preproduction.md
├── incs
│   ├── GameState.hpp
│   ├── IGraphic.hpp
│   └── Input.hpp
├── libs
│   ├── ncurses
│   ├── raylib
│   ├── sdl2
│   └── testlib
│       ├── compile.sh
│       ├── main.cpp
│       ├── plugin.cpp
│       └── plugin.h
├── Makefile
└── srcs
    └── main.cpp
```

### The Initial Interfaces
Three main interfaces are built with the initial testing in mind: `GameState`, `IGraphic` and `Input`. At this point:
-  `GameState` just contains the basic **data structures** that will be managed by the game
- `IGraphic` is built as a pure abstract class with
	- basic methods:
		- init()
		- render()
		- pollInput()
	- `extern "C"` requirements
- `Input` just homes the Input `enum` that will be the intermediary between each library's hooks/events/methods.

`GameState.hpp`
```C++
#pragma once
#include <cstddef>

struct Vec2 {
	int	x;
	int	y;
};

struct SnakeView {
	const Vec2	*segments;
	std::size_t	length;
};

struct FoodView {
	Vec2 position;
};

struct GameState {
	int			width;
	int			height;
	SnakeView	snake;
	FoodView	food;
	bool		gameOver;
};
```

`IGraphic.hpp`
```C++
#pragma once
#include "GameState.hpp"
#include "Input.hpp"

class IGraphic {
	public:
		virtual ~IGraphic() = default;

		virtual void init(int width, int height) = 0;
		virtual void render(const GameState &state) = 0;
		virtual Input pollInput() = 0;
};

extern "C" {
	IGraphic *createGraphic();
	void destroyGraphic(IGraphic*);
}
```

`Input.hpp`
```C++
enum class Input {
	None,
	Left,
	Right,
	Quit,
	SwitchLib1,
	SwitchLib2,
	SwitchLib3
};
```

With this done, the next steps are:

3. Creating the **Dummy graphic library**
4. building the `dlopen` pipeline

### The Dummy Graphic Library (`.so`)
At this initial point, the main aim is to build the simplest dummy graphic library that both allows a testing setup and imposes some necessary managements asked by the project. This is to say that, in order to avoid having to debug both architecture bugs and graphic bugs at the same time, as well as a storm of crashes with hard to track origins.

So, with this in mind, a simple dummy `.so` should test:

- ABI boundaries
- `dlopen / dlsyn / dlclose` pipeline
- object lifetime across `.so`
- runtime switching.

And, functionally speaking, should:
- Print to stdout
- Maybe print the game state as ASCII
- return fake input.

In part 1 of this document, an example `testlib` was proposed as a theoretical/educational starting point, but pragmatism calls for a way more simplified approach that **inherits from `IGraphic`**, loses unneeded features (`IHostCallbacks`, bidirectional communicatin, STL usage and arithmetic examples).

```C++
#include "../../incs/IGraphic.hpp"
#include <iostream>

class TestLib : public IGraphic {
	public:
		void init(int width, int height) {
			std::cout << "[TestLib] init(" << width << ", " << height << ")" << std::endl;
		}

		void render(const GameState &state) {
			std::cout << "[TestLib] render: snake length - "
				<< state.snake.length << std::endl;
			
				if (state.gameOver)
					std::cout << "[Dummy] GAME OVER" << std::endl;
		}

		Input pollInput() {
			return Input::None;
		}
};

extern "C" IGraphic *createGraphic() {
	return new TestLib();
}

extern "C" void destroyGraphic(IGraphic *g) {
	delete g;
}
```

### Building the `dlopen` pipeline (i.e., Building a Test Main)
With the basic dummy `TestLib` in place, a basic `Main` can be written to test both the management of the `.so` file as well as the implementation itself of the dummy library. The basic initial main is:
```C++
#include "incs/IGraphic.hpp"
#include "GameState.hpp"
#include <dlfcn.h> // for dynamic linking
#include <iostream>

int main() {
	void *handle = dlopen("./testlib.so", RTLD_NOW);
	if (!handle)
	{
		std::cerr << dlerror() << std::endl;
		return 1;
	}

	using CreateFn = IGraphic *(*)();
	using DestroyFn = void (*)(IGraphic*);

	CreateFn create = (CreateFn)dlsym(handle, "createGraphic");
	DestroyFn destroy = (DestroyFn)dlsym(handle, "destroyGraphic");

	if (!create || !destroy)
	{
		std::cerr << "Symbol error" << std::endl;
		return 1;
	}

	IGraphic *gfx = create();
	gfx->init(20, 20);

	Vec2 segments[4] = {{10,10}, {9,10}, {8,10}, {7,10}};
	SnakeView snake { segments, 4 };
	FoodView food{ {5,5} };

	GameState state {
		20, 20, snake, food, false
	};

	gfx->render(state);

	destroy(gfx);
	dlclose(handle);
}
```

### Some considerations:

- What `dlopen` actually does:
    - Loads shared object into memory
    - resolves its symbols
    - returns a **handle** that can be queried with `dlsym`
- The second argument in the call to `dlopen` **controls when symbols are resolved**. There are two main options:
    - `RTLD_NOW`: Resolve all undefined symbols immediately
        - If *any* symbol is missing → `dlopen` fails
        - Problems are found **early**
        - Safer, more predictable
    - `RTDL_LAZY`: Resolve symbols only when first used
        - `dlopen` succeeds even if symbols are missing
        - Program may crash *later*, when calling a symbol
        - Harder to debug
- `dlopen` is called with `RTDL_NOW` because we want to detect, as per subject request (with neat exit):
    - Incompatible libraries
    - Missing functions
    - Wrong versions

- The type aliases via `using` are there because **in dynamic loading `dlsym` retruns `void*`**, so the return value **must be cast to the exct function pointer type expected**. The flow undergoes this process:
    - **Type Aiasses** are set up as readable names to what the symbols will return. 
        - The example `using CreateFn = IGraphic *(*)()` is comprised of:
            - `()` → takes no parameters
            - `*` → pointer to a function
            - returns `IGraphic*`
                - In human: `Pointer to a function that returns IGraphic * and takes no arguments`
        - The aliasses are used to cast the return of `dlsym`, which takes the `handle` (the dynamic library) and the symbol name and returns, as mentioned, a type `void*`.
        - If the signature is wrong (i.e., the aliassing is messed up), the result may be undefined behaviour, a good old crash (GOC) or plain memory corruption, so **correctness is critical here**.
        - To sum up, the matching occurs this way:
            - `using CreateFn = IGraphic *(*)();` - `extern "C" IGraphic* createGraphic();`
        - On the side of readible-ness, aliasses are used because something like `auto create = (IGraphic*(*)())dlsym(handle, "createGraphic");` would be hard to read and error prone.
        - Also, `using` is used instead of `typedef` because its clearer, better with templates and more **modern C++**

- Summing up::
```C++
using CreateFn = IGraphic *(*)();
CreateFn create = (CreateFn)dlsym(handle, "createGraphic");
```
> “At runtime, I will load a symbol named createGraphic. I promise it is a function that returns IGraphic*. I will call it through this pointer.”

Once this is in place, the last thing regarding Day One's is the Makefile tweaks so that it:
- Builds the `testlib` shared library file
- Compiles the binary that will link against the `.so` file
- Correcctly handles dependencies and all the etceteras that a Makefile should do

#### Building the `.so`
Nothing too fancy, nor too complicated. Your `.so` file building run-of-the-mill rules:
```make
LIB_CFLAGS		=	-Wall -Wextra -Werror -std=c++20 -g -O3 -fPIC $(INCLUDES)
LDFLAGS			=	-ldl
```
```make
$(LIB_NAME): $(LIB_OBJS)
	$(CC) -shared -o $@ $^
```

```make
$(LIB_OBJDIR)/%.o: $(LIBDIR)/%.cpp Makefile
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/libs/$(*D)
	$(CC) $(LIB_CFLAGS) $(DEPFLAGS) -c $< -o $@ -MF $(DEPDIR)/libs/$*.d
```

```make
$(NAME): $(OBJS) $(LIB_NAME)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)
	@echo "$(GREEN)Snakeboarding is not a crime$(DEF_COLOR)"
```
Annotations regarding some flags and used syntax:
- The `-fPIC` Flag (Position Independent Code):
    - **Required for .so files**: Code must be loadable at any memory address
    - Generates code that doesn't depend on being at a specific memory location
    - Slight performance overhead, but necessary for shared libraries
- The `-shared` Flag:
    - **Creates the shared library**: Tells the linker to produce a `.so` file isntead of an executable (similarly as how `arc rs` creates a `.a` file for static libraries, think of `libft` for example).
    - Combines all objects into a single shared library
    - The library can be loaded by multiple programs at runtime
- The `-ldl` Flag:
    - Links against `lbdl.so` (dynamic linkin library)
    - Needed because the code uses `dlopen()`, `dlsym()`, `dlclose()` functions
        - These functions allow **runtime** loading of shared libraries
        - Without -ldl, "undefined reference to dlopen" errors will be returned
- How the `.so` file and the binary work with each other:
    - The linking of the binary against the library is not done at compile time, instead, **the `.so` file must exist in the directory when the program is ran**
        - In other words, `Makefile` builds the `.so` files, compiles the binary, an then the actual using of the dynamic libraries is done via `dlopen` and friends:
            - The binary doesn't know about `testlib.so` at compile time (as required by subject)
            - At **runtime**, `dlopen()` loads the .so file
            - `dlsym()` finds functions inside it (via "C" like symbols, i.e., predictable symbols)
            - **Plugin architecture** is achieved, which means that libraries can we swaped without recompiling.

---
*To be honest, Day One's included a runtime switching test, but I feel that I was too ambitious when I wrote those and I'll leave that to, let's say, Day Two's. See you on the next document*