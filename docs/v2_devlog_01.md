# Rosario V2 - Devlog 01

**Date**: February 11, 2026  
**Focus**: V2 Initialization & Cleanup

## Overview
Today marks the transition from Nibbler (academic multi-library project) to Rosario (Raylib-unified showcase game). This is a strategic pivot focused on creating a polished prototype for the Larian Studios networking opportunity.

## Goals
- **Week 1**: Clean foundation - Raylib-only build with ported SDL features
- **Week 2**: Innovation - Unique mechanics (rosario/bead system, dimension switching)
- **End Goal**: Compelling 2-week prototype showcasing game design & polish skills

## Day 1: Repository & Architecture Decisions

### Key Decisions Made
1. **Repository Strategy**: Same repo, tagged V1, created `v2-rosario` branch
   - Preserves development history
   - Allows cherry-picking from V1
   - Clean separation via tags

2. **Architecture**: Refined OOP (NOT ECS)
   - ECS deemed overkill for snake game scope
   - Focus on game design over architectural patterns
   - Larian values innovation & polish over tech complexity

3. **What's Preserved from V1**:
   - ✅ Full AI system (SnakeAI, Pathfinder, FloodFill)
   - ✅ Multi-mode gameplay (Single, Multi, VsAI)
   - ✅ Core game logic (Snake, Food, GameManager)
   - ✅ Raylib 3D isometric renderer
   - ✅ SDL particle system concept (to be ported)
   - ✅ SDL menu design (to be ported 1:1 to Raylib)

4. **What's Removed**:
   - ❌ NCurses implementation
   - ❌ SDL implementation (after porting needed features)
   - ❌ Multi-library abstractions (IGraphic, IAudio, LibraryManager)
   - ❌ External library builds (libs/ directory)

### Cleanup Tasks Completed
- [x] Removed NCurses graphics files
- [x] Removed SDL graphics files (source preserved in git history for porting)
- [x] Removed audio abstraction (IAudio)
- [x] Removed library management layer
- [x] Removed external libs directory
- [x] Archived V1 documentation
- [x] Created V2 devlog structure

### Next Steps
- [ ] Update DataStructs.hpp (use std::optional<Snake> for snake_B)
- [ ] Update Makefile for Raylib-only build
- [ ] Fix compilation errors
- [ ] Port SDL menu to Raylib
- [ ] Port SDL particle system to Raylib

## Technical Notes

### GameState Refactoring
Current `Snake* snake_B` needs to become `std::optional<Snake>` for:
- Modern C++ style
- Clear intent (may or may not exist)
- Safer null handling

### SDL Features to Port
Priority porting list:
1. **Menu System**: 2D text, particle effects, tunnel background
2. **Particle System**: Reusable feedback system
3. **Text Rendering**: Square-based aesthetic

### Architecture for V2
```
Core: Snake, Food, GameManager (mostly unchanged)
Rendering: RaylibRenderer with Style3D/2D/ASCII modules
Game Feel: ParticleSystem, ScreenEffects, FeedbackSystem
Innovation: BeadSystem, DimensionSwitching
```

## Reflections
The strategic decision to skip ECS and focus on game feel is confidence-inspiring. Two weeks is tight, but achievable if we stay disciplined about scope. The Larian context clarifies priorities: demonstrate game design instincts, not engineering dogma.

---

**Tomorrow**: Complete cleanup, get Raylib-only build compiling, begin SDL menu port.
