#include <stdio.h>
#include <unistd.h> // for usleep

// Move cursor up N lines
#define MOVE_UP(n) printf("\033[%dA", (n))
// Erase current line
#define ERASE_LINE printf("\033[2K")

int main(void) {
    int printedLines = 5; // number of lines to overwrite
    // Sample data variables
    int temp = 60;
    int power = 120;
    int clock = 1500;
    int fan = 35;
    double used = 4.5;
    double total = 8.0;

    // Print initial data
    printf("Temp: %d C\n", temp);
    printf("Power: %d W\n", power);
    printf("GPU Clock: %d MHz\n", clock);
    printf("Fan: %d %%\n", fan);
    printf("Memory: %.2f / %.2f GB\n", used, total);

    while(1) {
        // get data with nvml

        // Move cursor up to overwrite previous output
        MOVE_UP(printedLines);

        // Print updated info
        ERASE_LINE; printf("Temp: %d C\n", temp);
        ERASE_LINE; printf("Power: %d W\n", power);
        ERASE_LINE; printf("GPU Clock: %d MHz\n", clock);
        ERASE_LINE; printf("Fan: %d %%\n", fan);
        ERASE_LINE; printf("Memory: %.2f / %.2f GB\n", used, total);

        // Sleep 1 second
        usleep(1000000);
    }

    return 0;
}