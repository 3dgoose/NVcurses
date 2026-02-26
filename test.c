#include <ncurses.h>
#include <unistd.h>  // for sleep

int main() {
    initscr();          // Start curses mode
    noecho();           // Don't echo input
    cbreak();           // Disable line buffering
    nodelay(stdscr, TRUE);  // getch() won't block
    keypad(stdscr, TRUE);   // Enable arrow keys

    int ch;
    int counter = 0;

    while (1) {
        clear();  // Clear screen each loop

        // Display your "live" data
        mvprintw(0, 0, "Performance Monitor Demo");
        mvprintw(2, 0, "Counter: %d", counter);

        refresh();  // Update screen

        // Handle input (non-blocking)
        ch = getch();
        if (ch == 'q') break;  // Quit

        // Increment example variable
        counter++;

        // Sleep for 1 second (or use finer timing)
        sleep(1);
    }

    endwin();
    return 0;
}
