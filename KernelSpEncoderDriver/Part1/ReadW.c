#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>  // For usleep function

#define CONSUMER "gpio-toggle"  // Name of the consumer (optional label)

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line23, *line18;
    int toggle = 0;  // Initialize toggle variable
    int input;

    // Open GPIO chip (e.g., gpiochip0)
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) {
        perror("Open gpiochip0 failed");
        return 1;
    }

    // Get line 23 (output) and line 18 (input/output)
    line23 = gpiod_chip_get_line(chip, 23);
    line18 = gpiod_chip_get_line(chip, 18);

    if (!line23 || !line18) {
        perror("Failed to get GPIO lines");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line 23 as output and line 18 as input/output
    if (gpiod_line_request_output(line23, CONSUMER, 0) < 0 ||
        gpiod_line_request_output(line18, CONSUMER, 0) < 0) {
        perror("Request GPIO lines as output failed");
        gpiod_chip_close(chip);
        return 1;
    }

    while (1) {
        // Toggle GPIO 23 every 0.1 seconds
        if (toggle == 0) {
            gpiod_line_set_value(line23, 1);  // Set GPIO 23 high
            toggle = 1;
        } else {
            gpiod_line_set_value(line23, 0);  // Set GPIO 23 low
            toggle = 0;
        }

        // Read the state of GPIO 18
        input = gpiod_line_get_value(line18);

        // If GPIO 18 is different from GPIO 23, set GPIO 18 to match GPIO 23
        if (input != toggle) {
            gpiod_line_set_value(line18, toggle);  // Set GPIO 18 to match GPIO 23
        }

        usleep(100000);  // Sleep for 0.1 seconds
    }

    // Clean up
    gpiod_line_release(line23);
    gpiod_line_release(line18);
    gpiod_chip_close(chip);

    return 0;
}
