#!/bin/zsh
# Valgrind memory leak checker for Nibbler

make && valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-out.txt \
    --suppressions=ncurses.supp \
    ./nibbler 30 30

echo "\n=== Valgrind output saved to valgrind-out.txt ==="
echo "=== Showing summary: ==="
tail -n 30 valgrind-out.txt
