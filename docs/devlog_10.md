# Nibbler - Devlog - 10

## Table of Contents
1. [Day Ten Plan](#101-day-ten-plan)
2. [Small Leaks and Great Ships](#)

<br>
<br>
<br>

## 10.1 Day Ten Plan
It has been a couple of days since the last time I worked in this. The project is quite done, but there's still some things that *need* to be done, as well as a bunch of others that *could* be done. I'll obviously tackle first the necessities in this tenth session, and once I feel that those are done I'll think about the potentialities. So, today's checklist contains:
- Leak check (and cleanup)
- Subject thorough revision
- Start and Gameover screen refinements

Once these are done, I'll have to merge my branch with my partern's, who's been taking care of sounds. And then we'll see.

<br>
<br>
<br>

 ## 10.2 Small Leaks and Great Ships
 I just googled "quotes with leaks" and something apparently said by Benjamin Franklin popped up. Even great ships sink under small leaks or something in that vein. And while I don't know (you tell me, be nice) if this is a Great Ship, I know it has some Small Leaks, so we have to put ourselves in the mind of the hunter. Seek and retrieve every ounce of lost memory, bring them home, make the project gr... Oops, I almost got too carried away. Scratch that. Forget about it. Let's just work.

 I've been doing some tests and it *looks like* all the leak logs were related to the graphic libraries, specifically how the track their internal and global states. Setting up some suppresion files for valgrind, all related to internal library functions, results in a report of 0 leaked units of memory, so I guess I'm good? Let me double check...

 ...yeah, my general conclussion is that all the traces thrown by Valgrind are related to **library-internal states** (specially from `SDL2`). This is happening, to the best of my understanding, because the core concept of the project, **dynamically linking the chosen graphic libraries** is something for which said libraries are not really designed. Unloading and loading them leaves some global states behind during a unique execution that inevitably get flagged by the leak checkers, and there's nothing I can do about it. (If there is, I'm not aware of it; if you are reading this and have some tips, they're always welcome).

 So, the best thing to do right now is to set up leak check pipelines with **valgrind suppresion files** that can be used to prove, upon discussion, that *our* program is leak free, and that every other consideration should be done regarding the libraries and the project's concept. To this end, a new `checks/` directory has been added to the project, containing check scripts and the `.supp` files for Valgrind. Running `nibbler` through the scripts should be enough for anyone assessing the work.

 There is a total of **4 scripts** with corresponding `.supp` files:
 - `check_leaks_ncurses.sh`
 - `check_leaks_sdl.sh`
 - `check_leaks_raylib.sh`
 - `check_leaks_unified.sh`

 This shall be enough.

 
