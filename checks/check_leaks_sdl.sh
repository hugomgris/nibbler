#!/bin/zsh
# Valgrind memory leak checker for Nibbler (SDL2 mode)

make && valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=checks/valgrind-sdl-out.txt \
    --suppressions=checks/sdl2.supp \
    ./nibbler 30 30

echo "\n=== SDL2 Valgrind output saved to valgrind-sdl-out.txt ==="
echo "=== Showing summary: ==="
tail -n 30 checks/valgrind-sdl-out.txt

echo ""
echo "SDL2 Leak Check Results:"
echo "=============================="

if grep -q "definitely lost: 0 bytes" checks/valgrind-sdl-out.txt; then
    echo "No definite leaks"
else
    echo "Definite leaks found:"
    grep "definitely lost" checks/valgrind-sdl-out.txt
fi

if grep -q "indirectly lost: 0 bytes" checks/valgrind-sdl-out.txt; then
    echo "No indirect leaks"
else
    echo "Indirect leaks found:"
    grep "indirectly lost" checks/valgrind-sdl-out.txt
fi

echo ""
echo "Full output saved to: checks/valgrind-sdl-out.txt"
