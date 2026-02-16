# V2 Refactoring - Larian Showcase Focus

## Week 1: Foundation ✓
### Day 1-2: Cleanup & Port
- [x] Remove NCurses/SDL source files
- [x] Remove libs/ directory  
- [x] Remove IGraphic, IAudio, LibraryManager
- [x] Port SDL menu to Raylib (1:1)
- [x] Update GameState (use std::optional<Snake>)
- [x] Get Raylib-only build working
- [x] Update Makefile for single-library build

### Day 3-4: Particle & Polish
- [x] Port SDL ParticleSystem to Raylib
- [x] Port text rendering pipeline
- [x] Verify AI mode works

### Day 5-7: Game Feel
- [x] Screen effects (shake, zoom)
- [x] Particle feedback
- [ ] Audio integration
- [ ] Camera transitions

## Week 2: Innovation ✓
### Day 8-10: Rosario Mechanic
- [ ] Design bead system
- [ ] Implement & iterate
- [ ] Polish feel

### Day 11-12: Dimension Switching
- [ ] Style transitions
- [ ] Gameplay integration

### Day 13-14: Showcase Prep
- [ ] Bug fixes
- [ ] Record trailer
- [ ] Prep demo
EOF

git add REFACTORING_TODO.md
git commit -m "docs: add Larian-focused V2 roadmap"