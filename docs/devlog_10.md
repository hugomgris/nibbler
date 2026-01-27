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

 One of the main culprits of the leaks is going to be, I'm sure, the pointer management. Because I was too focused on implementation and development of the project's main features, I forgot to put on my `std11+` brain (I started coding in `C++98`), so I didn't manage allocations with smart pointers. I'm dumb like that, yes, but at least this should be an easy fix.

 Up until now I've been ignoring the logs thrown by `fsanitize` when going through `SDL2`, but they have been trully difficult to follow. So I'll first switch to valgrind and make a general assessment.
