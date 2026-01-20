# Nibbler - Devlog - 7

## Table of Contents
1. [Day Seven Plan](#71-day-seven-plan)
2. [2D Graphics Are Difficult](#72-2d-graphics-are-difficult)

<br>
<br>
<br>

## 7.1 Day Seven Plan
I'm quite happy with the visuals reached today regarding the 3D realm. They're basic but look good, and I think they're more than enough for the task's requiremets. The problem at this point is that the other two realms, ASCII and 2D are quite behind in regards to a e s t h e t i c s, so I guess that's going to be the main focus of today's work. I'd really like to have this project fully done by the end of this week, so achieving an overall satisfactory visual level across libraries would be really helpful towards that goal. Then I'll be able to focus in entry and exit points and use whatever time I have left to add some extra gameplay sprinkles.

> Also, I've been increasingly obsessed about trying to take my shadow pupettering game prototype into Playdate, so I want to dedicate some time to it. And there is a ton of other things in my table, so the sooner I can reach a milestone with this snake, the better.

Anyways, day seven's general plan is:
- Enhance ASCII realm's visuals
- Enhance 2D realm's visuals
- If time:
	- Score tracking
	- Exit point expansion: game over popup, retry option, exit option, score visualization


> *Oops, I got sidetracked with a Playdate prototype I'm also going to build this days haha let's get back to this*

## 7.2 2D Graphics Are Difficult
Making a good looking 2D, flat version of snake is, contrary of what it may look like, quite complicated. Buildin a 3D setup with a couple o color choices and some postprocessing returns an easy good looking, geometrical visualization, but in 2D there are less tools. Or, at least, there are less tools that give a quick, general, satisfactory render.

My best route right now is to replicate something similar to what I did in my [*Pong* engine](https://github.com/hugomgris/pong), i.e., implement some particles and and line-based, depth-faking effects. The only problem is that back in the day, arriving to that aesthetic took me a couple of months (as always, I had to learn my tools before using them), so I'll have to make a quicker version and don't die in the process. 