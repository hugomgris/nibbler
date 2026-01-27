#!/bin/bash
# Unified leak checker for all graphics libraries
# Uses combined suppressions (all_libs.supp) to check any library

echo "Unified Leak Checker - All Libraries"
echo "========================================"
echo ""

# Build the project
echo "Building project..."
make -s
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo "Build successful"
echo ""
echo "Running Valgrind with unified suppressions..."
echo ""

# Run Valgrind with combined suppressions
valgrind \
    --leak-check=full \
    --show-leak-kinds=definite,indirect \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind-unified.txt \
    --suppressions=all_libs.supp \
    ./nibbler 30 30

echo ""
echo "Unified Leak Check Results:"
echo "=============================="
echo ""

# Check for leaks in user code
USER_LEAKS=$(grep -E "SDLGraphic|NCursesGraphic|RaylibGraphic|main\.cpp|Snake|Food|GameManager|LibraryManager" valgrind-unified.txt | grep -E "definitely lost|indirectly lost")

if [ -z "$USER_LEAKS" ]; then
    echo "No leaks detected in your code!"
    echo ""
    echo "Summary from valgrind-unified.txt:"
    grep "definitely lost" valgrind-unified.txt | head -1
    grep "indirectly lost" valgrind-unified.txt | head -1
    grep "suppressed" valgrind-unified.txt | tail -1
else
    echo "Leaks found in user code:"
    echo "$USER_LEAKS"
fi

echo ""
echo "Full output saved to: valgrind-unified.txt"
echo ""
echo "Tip: Use library-specific checkers for detailed analysis:"
echo "   ./check_leaks.sh          # NCurses only"
echo "   ./check_leaks_sdl.sh      # SDL2 only"
echo "   ./check_leaks_raylib.sh   # Raylib only"
