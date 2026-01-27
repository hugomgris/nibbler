#!/bin/bash
# Raylib-specific leak checker
# Focuses on Raylib, GLFW, and OpenGL memory management

echo "Running Valgrind on Raylib implementation..."

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-raylib-out.txt \
    --suppressions=raylib.supp \
    ./nibbler 30 30

echo ""
echo "Raylib Leak Check Results:"
echo "=============================="

if grep -q "definitely lost: 0 bytes" valgrind-raylib-out.txt; then
    echo "No definite leaks"
else
    echo "Definite leaks found:"
    grep "definitely lost" valgrind-raylib-out.txt
fi

if grep -q "indirectly lost: 0 bytes" valgrind-raylib-out.txt; then
    echo "No indirect leaks"
else
    echo "Indirect leaks found:"
    grep "indirectly lost" valgrind-raylib-out.txt
fi

echo ""
echo "Full output saved to: valgrind-raylib-out.txt"
