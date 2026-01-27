#!/bin/bash
# Raylib-specific leak checker
# Focuses on Raylib, GLFW, and OpenGL memory management

echo "Running Valgrind on Raylib implementation..."

make && valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=checks/valgrind-raylib-out.txt \
    --suppressions=checks/raylib.supp \
    ./nibbler 30 30

echo ""
echo "Raylib Leak Check Results:"
echo "=============================="

if grep -q "definitely lost: 0 bytes" checks/valgrind-raylib-out.txt; then
    echo "No definite leaks"
else
    echo "Definite leaks found:"
    grep "definitely lost" checks/valgrind-raylib-out.txt
fi

if grep -q "indirectly lost: 0 bytes" checks/valgrind-raylib-out.txt; then
    echo "No indirect leaks"
else
    echo "Indirect leaks found:"
    grep "indirectly lost" checks/valgrind-raylib-out.txt
fi

echo ""
echo "Full output saved to: checks/valgrind-raylib-out.txt"
