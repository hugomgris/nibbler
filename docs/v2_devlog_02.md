# Rosario - Devlog - 2

## Table of Contents
1. [Design Week Barcelona](#21---design-week-barcelona)
2. [The Walls Have Eyes](#22---the-walls-have-eyes)

<br>
<br>


# 2.1 - Design Week Barcelona
While developing `V1` and leaving a long trail of logs behind, a collection of different design ideas and possibilities piled up. Some of them more complex and more in need of further conceptualization, some derivative from other past projects of mine, but all in all a series of ways for thinking about twisting the original *Snake* foundations that the *nibbler/rosario* builds on. All of this aside, given the time constrains for this first incursion into `V2`, the design ideas of choice need to be selected with practicality in mind, all while also trying to pick them up with a showcasey mindset in mind. I think that in a past `V1` log I mentioned that my way of thinking about this *classic*, well rounded games was to break them appart into their basic elements and try to build on them. That's what I did with *Pong*, and what I'm aiming to keep doing in this first implementations. So, going back to *Snake*, it's core ingredients can be listed as:
- The `walls`
- The `snake`
- The `food`

On top of which we can also add some formal/material elements, somewhat superimposed, maybe a little bit meta, like:
- The representation paradigm (`2D`, `3D`)
- Some `textual` overlay
- The `UI`

We'll focus mainly in the first list, both because prototyping a couple of ideas in those 3 fronts in my current time frame seems doable. The first of the second tier, the aim to tie the visual mode with the mechanic layer of the game experience would also be great if it ended up included in the first prototype, but I fear that time is against me in this one, as it needs an overall rethinking about how the `3D` snake will work (I have some thoughts about it, but they are not *simple* ones, in the sense of their implementation). I'll try, nevertheless, and we'll see how it goes. And in this line, I might as well get right into it, straight to the first item of the first list, and work with the `2D` visualization wall elements. 

<br>
<br>

# 2.2 - The Walls Have Eyes
The first gameplay twist will be taken from my *Pong* game engine, as I think it translates perfectly to the `2D` realm of `V2`. The basic idea is to have **morphing walls**: having them move, grow, enclose the snakes progressively, reducing the game arena, blocking some paths, etc. The visual foundation for this is already in place: **the white walls and the animated tunnel-effect, background lines**. The walls and their transformation will mark the current area of the game, the safe places for the snakes, while the lines will *"announce"* their change. Moreover, and still in the same line as that mentioned *Pong*, these changing obstacles can be implement in (at least) two ways: **transforming walls** and **spawning obstacles**. Something about which a couple of images would explain far more and better than me keeping writing my way into your brain:

<img src="pong01.png" alt="Pong screenshot with non-wall obstacles">
<img src="pong02.png" alt="Pong screenshot with wall-growth obstacles">

From here, the porting of these mechanics needs to take into account how the rendering and overall game management is done in this new, `C++` based engine that I've been building. Back there, in *Pong*, I remeber this process to be quite difficult, but the second time should be easier, right? R I G H T ? ? ? Well, we'll see. Some steps/doubts/general-ideas from the top of my head that sound important to lay out before diving into coding:
- The `WALLS` side:
	- Making the walls grow in a *snake* game, which is navigated (at least in this one, right now) orthogonally, calls for some building/visual organization to sanily tie how walls are built and how they grow. In less cryptic words, how the walls are drawn is going to need to change: **from a 4 rectangle based wall construction to a square-based layout**. Even more basically: **we'll now draw walls with square bricks**. 
	- Why `bricks`? Because we want to link everything to the same managing unit: a wall brick, a snake segment, a food pip, ... Everything should be based on squares to avoid interaction issues down the line.
	- Furthermore, **the bricks should be sized with divisable magnitudes** to layout the less painful possible ground for animation. Think about a wall that triggers a one brick worth of growth in some of it's points, like a lump. Said lump would be announced by a previous, gradual growth in the background tunnel lines, which will lerp it's formation during the spawn time. Say that that lump takes 8 background lines to completely form, each of them making the wall lump itself, the solid, white and collisionable one, slightly form. Therefore, having the square be divisable by 8 is paramount. We can start with `64x64` sized bricks.
- The `Tunnel Lines` side:
	- The lines that announce changes are going to need some rework to be able to be finely manipulated. Again, at this point they're drawn with `Raylib` line rendering functions, but they're going to have to be split into units to be able to twist, grow and transform.
	- What this means, also, is that the `TunnelLine` struct that I've been working with is going to need some (re)development, maybe even a transformation into its own class.
	- Everything will be manually managed with coordinate tracing, but I'll try my best to automate the new processes as much as possible.

Let's get to work!

