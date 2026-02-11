# V2 Refactoring - Larian Showcase Focus

## Week 1: Foundation ✓
### Day 1-2: Cleanup & Port
- [ ] Remove NCurses/SDL source files
- [ ] Remove libs/ directory  
- [ ] Remove IGraphic, IAudio, LibraryManager
- [ ] Port SDL menu to Raylib (1:1)
- [ ] Update GameState (use std::optional<Snake>)
- [ ] Get Raylib-only build working

### Day 3-4: Particle & Polish
- [ ] Port SDL ParticleSystem to Raylib
- [ ] Port text rendering pipeline
- [ ] Verify AI mode works

### Day 5-7: Game Feel
- [ ] Screen effects (shake, zoom)
- [ ] Particle feedback
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