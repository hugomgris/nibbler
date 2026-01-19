#include <ncurses.h>
#include <stdio.h>

int main() {
    printf("isendwin() before initscr: %d\n", isendwin());
    initscr();
    printf("isendwin() after initscr: %d\n", isendwin());
    endwin();
    printf("isendwin() after endwin: %d\n", isendwin());
    return 0;
}
