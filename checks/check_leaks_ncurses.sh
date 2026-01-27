#!/bin/zsh
# Valgrind memory leak checker for Nibbler

make && valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=checks/valgrind-ncurses-out.txt \
    --suppressions=checks/ncurses.supp \
    ./nibbler 30 30

echo ""
echo "NCurses Leak Check Results:"
echo "=============================="

if grep -q "definitely lost: 0 bytes" checks/valgrind-ncurses-out.txt; then
    echo "No definite leaks"
else
    echo "Definite leaks found:"
    grep "definitely lost" checks/valgrind-ncurses-out.txt
fi

if grep -q "indirectly lost: 0 bytes" checks/valgrind-ncurses-out.txt; then
    echo "No indirect leaks"
else
    echo "Indirect leaks found:"
    grep "indirectly lost" checks/valgrind-ncurses-out.txt
fi

echo ""
echo "Full output saved to: checks/valgrind-ncurses-out.txt"
