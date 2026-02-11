# Rosario (Nibbler V2)

**Extended Snake Game** - Raylib-unified showcase project

## Version History

- **V1** (Academic): Multi-library snake implementation → See [`v1.0-academic`](https://github.com/hugomgris/nibbler/tree/v1.0-academic) tag
- **V2** (Rosario): Raylib-unified extended game with innovative mechanics *(current)*

## Current Status: Early V2 Development

**Phase**: Foundation & Cleanup  
**Branch**: `v2-rosario`  
**Focus**: Building polished 2-week prototype showcasing game design & innovation

### What's New in V2
- 🎮 **Raylib-only** - Simplified architecture, single graphics library
- 🤖 **Enhanced AI** - Full pathfinding AI opponent (from V1)
- 🎨 **Multiple Visual Styles** - 3D isometric, 2D flat, ASCII (dimensional switching)
- 🔮 **Rosario Mechanic** - Innovative bead-chain system
- ✨ **Game Feel** - Particles, screen effects, polish

### Preserved from V1
- Full AI system (SnakeAI, Pathfinder, FloodFill)
- Multi-mode gameplay (Single, Multi, VsAI)
- Core game logic

## Building

```bash
make
./nibbler [width] [height]
```

*Note: Build system currently broken - being updated for Raylib-only in next commit*

## Development

See [`docs/v2_devlog_01.md`](docs/v2_devlog_01.md) for current development status.

## Project Structure

```
srcs/
├── AI/              # Pathfinding AI system
├── graphics/        # Raylib rendering
├── Snake.cpp        # Core game logic
├── Food.cpp
├── GameManager.cpp
└── main.cpp

incs/               # Header files
tests/              # Unit & integration tests
docs/
├── v2_devlog_*.md  # V2 development logs
└── v1_archive/     # V1 documentation
```

## License

Academic project - 42 School
