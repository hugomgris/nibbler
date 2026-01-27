#!/bin/zsh
# Valgrind memory leak checker for Nibbler (SDL2 mode)

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-sdl-out.txt \
    --suppressions=sdl2.supp \
    ./nibbler 30 30

echo "\n=== SDL2 Valgrind output saved to valgrind-sdl-out.txt ==="
echo "=== Showing summary: ==="
tail -n 30 valgrind-sdl-out.txt
