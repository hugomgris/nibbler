# Nibbler - Devlog - 11

## Table of Contents
1. [Day Eleven Plan](#111-day-eleven-plan)
2. [The Miracle of Standards](#112-the-miracle-of-standards)
3. [The Graphic Gardens of Eden](#112-the-graphic-gardens-of-eden)
4. [Encouraging, Which Is a Gerund]

<br>
<br>
<br>

## 11.1 Day Eleven Plan
A considerable chunk of today's working time has gone towards job applications, so I don't think there will be great advancements today. Still, there are a couple of hours left on the workclock, so let's put them to good use. Today's plan doesn't need a list, as it is quite straight forward: **working on the start and gameover screens** across platforms.

> *As I wrote this devlog, it seems that I fully adopted a snake + apple = garden of eden theme. By day 11, one must find ways to keep things joyful, right?*

> *This log ended up being about more than one day: from the 11th to the 14th work session. I don't want to change too much stuff here, so I'll just make the 12th devlog tart with *Day Fifteen Plans*.

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

<p float="left">
  <img src="ncurses_gameover_big.png" alt="NCurses game over screen in its big format" height=500>
  <img src="ncurses_gameover_small.png" alt="NCurses game over screen in its small format" height=500 hspace="40">
</p>

Now, let's move on to the 2D realms of `SDL2`. This will first need a somewhat of homogenization (*however this cursed word is written*) pass, as I'd like for the main logic for the manus and middle screen to be the same across platforms. 

BEAKING NEWS! I've been working on the game's title/logo/whateveryouwantocallit. While drawing the `SDL`start screen, I started playing around with the idea of the `i` of `nibble` be the snake, blue body and red dot, and after a couple of iterations I arrived to a version that, at least for now, I like. I also implemented some particles as a trail for the snake, which are included in the title. So after doing this, I went back to `NCurses` to modify its logos to h o m o g e n i z e (*or however this cursed workd is written*) the a e s t h e t i c s. So here's the current state:

<p float="left">
  <img src="NCurses_logo2_small.png" alt="NCurses game over screen in its big format" height=500>
  <img src="NCurses_logo2_big.png" alt="NCurses game over screen in its small format" height=500 hspace="40">
</p>

<br>

Anyway, working in `SDL2` is been somewhat of a (small) challenge. I decided to draw by hand the logo, before moving into researching how to render text. I learned that a new sub-library needed to be handled, `SDL_ttf`, so I had to add the process of its fetching to `Makefile`. After that, the basic flow of text rendering was easy to get around (plenty of examples out there). I tackled this by writing an specific function dedicated to render text to serve as helper, but because things grew too much, I ended up dettaching the logic from `SDLGraphic` into it's own class (did the same for the particle System):

```cpp
#ifndef TEXTRENDERER_HPP
#define TEXTRENDERER_HPP

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>
#include "colors.h"

static constexpr SDL_Color customWhite{255, 248, 227, 255};  // Off-white
static constexpr SDL_Color customGray{136, 136, 136, 255};   // Gray

class TextRenderer {
private:
	SDL_Renderer* renderer;
	TTF_Font* mainFont;
	TTF_Font* smallFont;
	bool initialized;

public:
	TextRenderer(SDL_Renderer* renderer);
	~TextRenderer();

	bool init(int windowWidth);

	bool renderText(const std::string& text, int x, int y, int offset, 
	                TTF_Font* fontToUse, SDL_Color color, bool centered = false);

	void renderInstruction(int centerX, int centerY, int& offset,
	                       const std::string& labelText, const std::string& dotText,
	                       bool smallMode, TTF_Font* currentFont);

	void renderScore(int centerX, int centerY, int score, bool smallMode);
	void renderRetryPrompt(int centerX, int centerY, bool smallMode);

	TTF_Font* getMainFont() const { return mainFont; }
	TTF_Font* getSmallFont() const { return smallFont; }
	bool isInitialized() const { return initialized; }
};

#endif
```

The core of the text rendering pipeline is this function:
```cpp
bool TextRenderer::renderText(const std::string& text, int x, int y, int offset, 
								TTF_Font* fontToUse, SDL_Color color, bool centered) {
	if (!fontToUse || !initialized) return false;

	SDL_Surface* surface = TTF_RenderUTF8_Blended(fontToUse, text.c_str(), color);
	if (!surface) {
		std::cerr << "Text render error: " << TTF_GetError() << std::endl;
		return false;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		SDL_FreeSurface(surface);
		return false;
	}

	SDL_Rect destRect;
	if (centered) {
		destRect = {
			x - (surface->w / 2),
			y - (surface->h / 2) + offset,
			surface->w,
			surface->h
		};
	} else {
		destRect = { x, y + offset, surface->w, surface->h };
	}

	SDL_RenderCopy(renderer, texture, nullptr, &destRect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}
```

Moving forward, I decided to draw both the title and the gameover of the SDL2 realm by hand, using geometry functions. Two reasons for this: 1) absolute control of the looks across game sizes and resolutions, 2) couldn't find a font that satisfied my needs, and making one by myself would take too much time. The general idea with the typography for these is to write words with snake-like squiggles, i.e., letters that could be written by moving the snake. I arrived at a point which is satisfactory enough, but might still evolve during development:

<p float="left">
  <img src="SDL_title_big.png" alt="SDL intro screen in its big format" height=500>
  <img src="SDL_gameover_big.png" alt="SDL gameover screen in its big format" height=500 hspace="40">
</p>

There's no need to change typographies here because the words are, as I said, geometry based, so I can tweak the size of them by just changing the building block's size (`square` and `separator`). In `NCurses` I had to design two different ASCII based options because dynamically writting them char by char would literally end me. Aside from this, the title and gameover drawing functions in SDL were too big and, again, were cluttering my class code, so I made a secondary class `TitleHandler` to, you guessed it, handle the title renderings.

> I think I should/could make this helper classes be cross-plataform, i.e. work for every library, but I'll leave that for later. If I end up animating NCurses, the same goes for `ParticleSystem` (maybe even a future, upper-layered `AnimationSystem`, who's to say).

> I also made a slight tweak in the snake size handling. I had a hardcoded max size of 100 up until now, but that limit should be, at least for a basic `snake` recreation, equal to the game arena area - 2 area units. So I did that :D

<br>
<br>

## 11.4 Encouraging, Which Is a Gerund
This section's title is just another byproduct of me losing my mind. *"Animando, que es gerundio"*, blended through Google Translator just as a joke. Anyway, I'd like for my menu screens to be animated at some point (something e x t r e m e l y time dependent), but just in case I actually find the time to make it happen, some ideas:
- `NCurses`
	- Change the dithered parts of the big logo, randomizing them in the animation loop, making it look glitchy. (Why glitchy? I don't know, it might just be because it combines two important features: *cool* and *doable*)
	- Make the squiggles of the small logo slightly twitchy, growing and shrinking in the animation loop
- `SDL2`
	- Try to mimic the border animation in the letters (*this might break my spirit*)