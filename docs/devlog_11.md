# Nibbler - Devlog - 11

## Table of Contents
1. [Day Eleven Plan](#111-day-eleven-plan)
2. [The Miracle of Standards](#112-the-miracle-of-standards)
3. [The Graphic Gardens of Eden](#112-the-graphic-gardens-of-eden)

<br>
<br>
<br>

## 11.1 Day Eleven Plan
A considerable chunk of today's working time has gone towards job applications, so I don't think there will be great advancements today. Still, there are a couple of hours left on the workclock, so let's put them to good use. Today's plan doesn't need a list, as it is quite straight forward: **working on the start and gameover screens** across platforms.

> *As I wrote this devlog, it seems that I fully adopted a snake + apple = garden of eden theme. By day 11, one must find ways to keep things joyful, right?*

<br>
<br>

## 11.2 The Miracle of Standards
When I first started this project I was fully focused on the implemententation of a general design, so I didn't pay much attention to the `C++` standarization. Because when I first learned this language I was bound to the `std98`, I still don't have completely automatized some of the tools provided by more recent standards, so now that we have a working build I think it is a good time to make a general assessment of the code and try to bring things as closer to de `std20` as I can.

In this regard, I'll start by making some crucial changes in `Main`:
1. Instead of having a plain old `const char *` array to store the `so` file paths, I'll move to a `constexpr std::array<std::string_view, 3>`
	- *This will reduce overhead because of the `constexpr` and will grant type safety, as `std::array` know its size at compile time*
	- *`std::string_view` will, on top of that, assure that there is no allocation, as this makes the whole data be comprised of just **a pointer + length (basically, same as `const char*`)***
	- *This also provides range-based iterator possibilities an bound checking, which I'm using right now but the future is impenetrable for the eyes of a mere mortal*

2. Instead of having a pointer based `GameState`, which I feel I went that way due to some `C`-like autopiloting inside my brain, I'll handle the contents of this struct via references. 
	- *This way there will not be null pointer concerns and ownership will be clearer (`Main` will own objects)*
```cpp
struct GameState {
    Snake& snake;
    Food&  food;
    // Removed unused foodChar field
    // ...
};
```
3. The array used for snake segments could use some `std::array` magic too, for the same reasons as point 1
4. Every `enum` I had would be better off being an `enum class`. For example, instead of having an `enum direction`, switching to an `enum class Direction`
	- *This needs some adaptations regarding the accessing of the enum itself, so that they go through *scoped names* (i.e., `Direction::UP`)*
	- *This provides tape safety, eludes namespace polluton and gives a layer of consistency with `GameStateType`, which was already an `enum class`
5. In the particle and background vectors in `SDL` could be enhanced by simply reserving the necessary memory for their containers. A `particles.reserve(maxDustDensity)` and a `borderLines.reserve(100)` in `init()` will suffice
	- *This is mainly to avoid reallocations when spawning, which greatly impacts performance (maybe not noticeably in the current build, but if the amount of spawning grows in the future, this will protect performance against said growth)*
6. In regards to performance, my random number generation was poopy. I did some research and noticed that I was repeating the creation of `std::random_device` and `std::mt19937` every time I generated a random int, so the logical thing to better this was to make every generation-dependent function go through a shared, well, generator.
	- Beyond what could be a massive performance improvent (again, if the game was bigger or after it grew, this could tank it), this also adds to the thread-safe initialization.
7. In `NCurses` menu handling (start and gameover screens), I was opening and reading the base files which contain the logo art every time I rendered them. This is a 101 case of *please, cache me, for the love of god*, so that's what I did.
	- *This needed the adition of a handful of new attributes inside the `NCursesGraphic` class, which are now feeded inside `init()`, as well as some rewriting of the draw functions to use the cached data instead of the base files*
	- *Again, we're in optimization mode, but this was particularly important because up until now every `NCurses` rendering call to menu related stuff was tied to a file I/O. For a lack of a better word, and risking myself into repetition: pooopy*
8. All colr declarations n `SDL2` where switched to `static constexpr` so that they are handled as **compile-time constants**, avoiding **runtime initialization**

And that's it for modernizations for now. I'll try to avoid the need to going back to this reviews from now on by switching my brain to `std20 = true`, but I'm only human, after all, I'm only human...

<br>
<br>

## 11.3 The Graphic Gardens of Eden
Snake, apple... Eden vibes, right? Whatever, there's design work to do...

First, let's try to finish the `NCurses` implementations by redesigning the gameover screens, both for small and large formats. I'll just reuse what I wrote for the start screen and use it to also draw the ending statuses. Here they are:

<p float="center">
  <img src="ncurses_gameover_big.png" alt="NCurses game over screen in its big format" height=400>
  <img src="ncurses_gameover_small.png" alt="NCurses game over screen in its small format" height=400>
</p>

Now, let's move on to the 2D realms of `SDL2`. This will first need a somewhat of homogenization (however this cursed word is written) pass, as I'd like for the main logic for the manus and middle screen to be the same across platforms. 
