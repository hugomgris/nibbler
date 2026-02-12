# Rosario - Devlog - 1

## Table of Contents
1. [It Was Just a Phase](#11---it-was-just-a-phase)
2. [Back to the Cocoon (Which Would Make More Sense if the Main Entity of the Game Was a Worm Instead of a Snake, But You Get the Idea)](#12-back-to-the-cocoon-which-would-make-more-sense-if-the-main-entity-of-the-game-was-a-worm-instead-of-a-snake-but-you-get-the-idea)
3. [The Snake and the Mouse](#13-the-snake-and-the-mouse)


<br>
<br>

# 1.1 - It Was Just a Phase
Here we are, on the other side of what has been tagged as `V1`. Today the two week sprint towards a more mechanically complex, raylib-unified, *snake* based game start, which means that we're having **R E F A C T O R I N G**  for breakfast, lunch and dinner. I've been in this position before, having to plan and layout carefully thought steps to scale *down* a build so that a new development process can start anew from a compact, controlled state of things. Personally, I still find this situations a bit overwhelming, mostly because after spending so much time writing a game that builds and launches and works and doesn't break and explode into pieces, taking it appart feels like playing a risky game of *Jenga*. Or maybe the analogy is not quite precise, because once you take out one piece the whole thing collapses and the next hours are comprised of a mixture of "might as well keep getting all the stuff I think should be out of the new starting point" and "how can I make this compile again for the love of everything sacred". I've being a little overdramatic, I know (must say that if you've arrived to this log after going through all the development journaling done for `V1`, you already know the extent of my dramatism), but it can get tyring. Theres a counterpoint, of course, the same one that's always looming in any programming journey: nothing works until it works, and when it works life becomes wonderful. So, yeah:
- today's first task is **scaling down and stripping the project of what has become dead weight under the banner of `V2`**
- then, a new build pipeline and some refactoring in the surviving code needs to be done so that a raylib-based game persists
- after which, some porting needs to be done, specially regarding `SDL` stuff
    - for now, I want what was the `SDL` menu in `V1` to be the main menu in `V2`, which entrails adding a `2D` rendering mode/pipeline
    - this means that the particle system, which was tied exclusively to `SDL`, needs to be ported to `Raylib`
- we'll end with a general check, a test suite reconfigration to adapt it in the new `Makefile` and a new, fresh starting point.

If everything goes right, at the end of today there will be a **raylib-exclusive *nibbler* build with the SDL starting menu and gameover and a recovered gtest suite**. If not... doesn't matter, because we will **S U C C E E D**.

> *The good news is that scaling down means also simplifying, which brings solace to the soul. Soulace, if you will.*

<br>
<br>

# 1.2. Back to the Cocoon (Which Would Make More Sense if the Main Entity of the Game Was a Worm Instead of a Snake, But You Get the Idea)
On second thought, it wouldn't even make that much sense if it was a worm. If it was a butterfly, going *back* to the cocoon would totally make sense, but worm would go *foward* to a cocoon. Uhm, anyway, here's a list of things that we're going to lose along the way:
- Anything `SDL` and `NCurses` related
- The graphic interfacing (no longer needed in the new monolibrary approach)
- Any audio related stuff (will rebuild audio inside `Raylib`)

All of this with the consequential changes each removal has in the code. Once done, a new `Makefile` will be written, and we *should* be good... And we are!! If you go through the codebase, you'll see that we're now `Raylib` exclusive and that the game builds and runs smoothly. Yay!

The more tricky part is what comes next, the porting of some `SDL` stuff into Raylib. We'll need a couple of things:
- A 2D-camera based rendering pipeline
- A rewriting of the `ParticleSystem` and the `menu`/`gameover` renderings into `Raylib`
- A general reorganization of the directory structure, some renaming, that kind of managing work...

After some work, I've arrived to a new base build for `V2`, a combination of what was already in place for the `Raylib` version in `V1` with the menuing of `SDL`. Making this work was a hustle, but the byproduct is that I now have implemented a `2D` rendering pipeline alongisde the existing `3D` one, which is nice progress. I've also divided the `srcs` directory intro `AI`, `core` and `graphics`, and will move on from here with a system-based approach in mind. This might mean some more refactorization, as the current division between `Renderer`, `TextRenderer` and `TitleHandler` feels a little bit wonky, but we'll see. 

Be what it may, what I'd like to do with the rest of the day, now that a lot of porting and refactoring is behind me, is work in some new implementations (the **I R K***). Some things in my immediate list:
- A clickable button system for menuing
- A general menu system that handles start, pause and gameover screens
- A systematized pipeline to write stuff in the likes of the current game logo

BUT as I lay out plans, I realize something: I need a deep re-structuring process. The current build is quite messy: there are nested systems that are general-purpose, subsystems that need to be decoupled, uncertainty regardin ownership... So the priority shifts that way. Before we move on, there needs to be a refactoring so that **`Main` owns all systems and calls their update methods in the game loop depending on the state of the game**. And the initial system configuration is comprised of:

- [x] Renderer
- [x] Game controller
- [x] Particle system
- [x] Text system
- [ ] Animation System
- [ ] Menu system
- [ ] Custom text system/pipeline/subsystem- [ ] Custom text system/pipeline/subsystem

Inter-system communication will be laid out by passing references. If things get too complicated down the line, which I doubt but who am I to say, I'll transition into an event system with connecting lambda functions. I'll get into system rebuild mode. Wish me luck.

## 1.3 The Snake and the Mouse
Let's implement buttons. 