#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep function

void read_and_print(const char *path, const char *label) {
    FILE *fp;
    char buffer[256];

    // Open the sysfs file
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    // Read the contents of the sysfs file
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s: %s", label, buffer);
    }

    // Close the file
    fclose(fp);
}

int main() {
    while (1) {
        // Read and print the encoder position
        read_and_print("/sys/kernel/encoder/position", "Encoder Position");

        // Read and print the pulses per second (PPS)
        read_and_print("/sys/kernel/encoder/pps", "PPS");

        // Read and print the revolutions per minute (RPM)
        read_and_print("/sys/kernel/encoder/rpm", "RPM");

        // Wait for 1 second before reading again
        sleep(1);

        // Clear the screen for a cleaner output (optional)
        printf("\033[H\033[J");
    }

    return EXIT_SUCCESS;
}
