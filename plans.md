> *brought from devlog_14 in v1*

## 14.3 Planning Plans
The immediate course of action calls for a division in development phases. Phase 1 closes the academic chapter of the project, getting *nibbler* ready for submission. Phase 2 marks the beginning of the personal, portfolio-oriented work, where the game morphs from a multi-library proof-of-concept into a unified, polished experience. Both phases have different goals, different constraints, and different measures of success.

### Phase 1: Academic Closure (February 9-11)
The first phase is about **functional completion** and **submission readiness**. This means that the multiplayer mode needs to work, even if in a basic implementation, across all three libraries. The focus here is not polish, but coverage and correctness.

#### Core Implementation (February 9)
The bulk of the work in this phase lands on making the multiplayer functional in all three rendering contexts. `SDL2` is the natural starting point due to it's already set up particle systems and text renderers, which makes prototyping faster. Once the base implementation is solid there, porting the logic to `NCurses` and `Raylib` becomes a matter of adapting interface calls and input handling.

The main tasks for this day are:
- **Finish multiplayer implementation in `SDL2`**
	- Two snake instances (`snake_A`, `snake_B`)
	- Independent input processing (arrow keys vs WASD)
	- Visual differentiation (color-coded snakes and trails)
	- Collision detection between snakes
	- Proper handling of food collision (which snake eats what)

- **Port multiplayer to `NCurses`**
	- Adapt `pollInput()` to handle both player control schemes
	- Update `drawSnake()` to render both snakes with different color pairs
	- Adjust text rendering for score/status of both players

- **Port multiplayer to `Raylib`**
	- Add second snake rendering in isometric view
	- Handle input routing for both players
	- Update camera framing if needed for larger play areas

#### Polish and Submission Prep (February 10-11)
Once the multiplayer works across libraries, the focus shifts to making sure everything is stable and submission-ready:
- **Input buffer refinement**: Ensure both players' inputs are correctly queued and processed without conflicts
- **Collision edge cases**: Test and fix any remaining collision bugs (self-collision, wall collision, snake-to-snake collision)
- **Menu state handling**: Add a basic toggle or startup parameter to choose single player vs multiplayer (doesn't need to be fancy, just functional)
- **Documentation cleanup**: Update README with build instructions, controls for both players, and any submission-specific notes
- **Final testing**: Run through the game in all three libraries, both single and multiplayer modes, to catch any last-minute issues

At the end of this phase, *nibbler* as an academic project is complete and ready to turn in.

<br>

### Phase 2: The Raylib Transition (February 12-24)
This is where the project transforms. Phase 2 is about **consolidation**, **refactoring**, and **ambition**. The multi-library approach served its purpose, but from this point forward the game needs to breathe as a unified experience. `Raylib` becomes the foundation, and the different visual representations become rendering modes within a single, cohesive architecture.

The timeline for this phase spans roughly two weeks, which at 60 hours a week gives us around 120 hours of development time. This is substantial, but not infinite, so prioritization is key. And I need to survive the process, let's not forget about it. This will be the nth sprint in a series of sprints that I've been chaining for a couple of years now, so let's try not to die. Alive and with a game: the ultimate objective.

#### Week 1: Foundation and Refactoring (February 12-18)
The first week is structural. It's about tearing down the multi-library scaffolding and building the new foundation without losing what works.

**Day 1-2: Branch and Consolidate**
- Create a `v2` branch to preserve the original multi-library implementation
- Strip out `SDL2` and `NCurses` libraries and their build processes from `Makefile`
- Centralize everything under `Raylib`
- Refactor `IGraphic` interface to fit a single-library context (might not need the interface anymore, but keep it for now in case we want to experiment with other engines later)

**Day 3-4: Visual Style System**
This is the conceptual leap. Instead of three libraries, we now have three (or more) visual styles, all rendered through `Raylib`. The implementation of a `VisualStyleManager` becomes the centerpiece:
```cpp
enum class VisualStyle {
    Isometric3D,    // Current Raylib 3D cubes
    Flat2D,         // Recreate SDL's square-based look
    ASCII,          // Texture-based monospace ASCII
};
```
Each style has its own rendering pipeline, but they all share the same game state, input handling, and logic. This means:
- **2D Mode**: Use `Camera2D`, draw snakes as colored squares, flat food sprites
- **ASCII Mode**: Render to a texture using a monospace font, maybe play around with some postprocessing (YES I WANT TO GO FULL CRT AGAIN BUT I'LL TRY TO RESTRAIN MYSELF)
- **3D Mode**: Keep the current isometric cube rendering

The key here is smooth transitions. No window destruction, no flickering. Just a camera shift, a render mode change, and maybe a brief animation. (Well, first there will be an auto-switch, an instant change. Then we'll see how to polish the transition, but planning is ever-able, ambition-embracing, dream-catching).

**Day 5-7: Gameplay Configuration System**
With the visual consolidation in place, the next step is building the `GameConfig` system to handle gameplay variants. This is where the game becomes more than a tech demo:
```cpp
struct GameConfig {
    enum class GameMode { SinglePlayer, VersusAI, Versus1v1 };
    enum class Difficulty { Easy, Normal, Hard };
    
    GameMode mode = GameMode::SinglePlayer;
    Difficulty difficulty = Difficulty::Normal;
    bool wallWrapping = false;
    bool friendlyFire = false;
    float gameSpeed = 1.0f;
    // More options as needed
};
```
This struct feeds into the `GameManager`, which uses it to drive gameplay rules. For example:
- If `wallWrapping` is true, hitting a wall teleports the snake to the opposite side instead of triggering game over
- If `friendlyFire` is true, snakes can collide with each other; otherwise they phase through
- `gameSpeed` modifies the update frequency, making the game faster or slower

This also means refactoring collision detection and movement logic to be config-aware. The goal is flexibility without bloat. Points for maintaining sanity.

#### Week 2: Features, Polish, and Deployment (February 19-24)
The second week is about bringing the game to life. Features, juice, and presentation.

**Day 8-10: AI Implementation**
One of the big additions for the portfolio version is an AI opponent. This doesn't need to be perfect, but it needs to be interesting. A basic pathfinding AI that can chase food and avoid obstacles is a good start. For extra depth, implement difficulty levels:
- **Easy AI**: Simple greedy pathfinding, sometimes makes mistakes
- **Normal AI**: Better pathfinding, avoids traps
- **Hard AI**: Anticipates player movements, plays aggressively

The AI should plug into the existing `Snake` class via a controller pattern, so that switching between human and AI control is seamless. *At least, that is the idea*.

**Day 11-12: Interactive Menu System**
The start screen evolves from a static title into an interactive menu. This is where `GameConfig` options become user-facing:
- **Play Solo**: SinglePlayer mode
- **Play vs AI**: VersusAI mode, with difficulty selection
- **Play 1v1**: Versus1v1 mode, local multiplayer
- **Options**: Toggle wall wrapping, friendly fire, game speed, visual style
- **Quit**: Exit the game

Menu navigation should be intuitive (arrow keys, Enter to select) and visually consistent with the current aesthetic (snake-based typography, particle effects, etc.).

**Day 13-14: Dimension-Switching as Mechanic**
Here's where the concept gets interesting. The dimension-switching isn't just a visual gimmick anymore—it becomes a gameplay mechanic. Different dimensions have different rules:
- **ASCII Dimension**: Wall wrapping enabled, faster game speed, retro CRT effects
- **2D Dimension**: Standard rules, balanced gameplay
- **3D Dimension**: Isometric view, maybe obstacles or gravity-based mechanics

Power-ups could force dimension shifts, or the player could choose to switch mid-game for strategic advantage. This is experimental, but it's also what makes the game unique.

**Day 15-16: Polish and Juice**
The game needs to *feel* good. This means:
- **Particle effects**: Enhanced trails, explosions on food pickup, dimension shift effects
- **Screen shake**: Subtle camera shake on collisions
- **Sound design**: Basic audio feedback for movement, eating, collisions, dimension shifts (if time allows, and all derived from what my *nibbler* partner adds before submission)
- **Animations**: Smooth transitions between game states, menus, and visual styles

**Day 17-18: Web Deployment**
`Raylib` has excellent WASM support, which means the game can run in a browser. This is huge for portfolio purposes. The steps:
1. Compile the game to WASM using `emscripten`
2. Host it on `itch.io` or a personal site
3. Test across browsers (Chrome, Firefox, Safari)
4. Optimize for load times and performance

Having a playable web link in two weeks time would be incredible, but also extremely ambitious. Let's aim for it but not lose ourselves in the way. Therefore, and because two weeks is tight for everything outlined above, if time runs short, priorities are:
1. **Visual style system** (2D and ASCII modes functional)
2. **GameConfig system** (at least wall wrapping and game mode selection)
3. **Interactive menu** (functional, even if not fully polished)
4. **AI opponent** (basic implementation, difficulty levels if time allows)
5. **Web deployment** (critical for portfolio impact)

In the land of hopes and dreams, an example of playful experimentation with the basic *snake* components would also be showcased. Something along the lines of **different types of food**, **warping walls and obstacles**, an initial prototyping of the **bead-based avatars**. This is surely not going to happen, but I like to be ambitious. (And I usually eat those ambitions, but WILLING is the first step towards WAYING, which is a silly way of referencing the good old *where there's a will, thre's a way*. I've been writing this document for the most part of a Sunday, and the rest of the day's time has gone into cleaning my house. I'm close to losing it.)