#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>  // For usleep function

#define CONSUMER "gpio-event-flip"
#define TIMEOUT 100000000  // 100 milliseconds for 10Hz oscillation

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line17, *line18, *line23;
    struct timespec ts = { 0, TIMEOUT };  // Timeout structure for gpiod_event_wait (100 ms)
    struct gpiod_line_event event;
    int toggle23 = 0;  // Toggle state for GPIO 23 (oscillating)
    int toggle18 = 0;  // Toggle state for GPIO 18

    // Open GPIO chip (e.g., gpiochip0)
    chip = gpiod_chip_open_by_name("gpiochip0");
    if (!chip) {
        perror("Open gpiochip0 failed");
        return 1;
    }

    // Get lines for GPIO 17 (input for event detection), GPIO 18 (output), and GPIO 23 (oscillating output)
    line17 = gpiod_chip_get_line(chip, 17);
    line18 = gpiod_chip_get_line(chip, 18);
    line23 = gpiod_chip_get_line(chip, 23);  // GPIO 23 will oscillate at 5Hz

    if (!line17 || !line18 || !line23) {
        perror("Failed to get GPIO lines");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line 17 to detect both rising and falling edges (input for event detection)
    if (gpiod_line_request_both_edges_events(line17, CONSUMER) < 0) {
        perror("Request GPIO 17 as input with events failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line 18 as output (to be toggled)
    if (gpiod_line_request_output(line18, CONSUMER, 0) < 0) {
        perror("Request GPIO 18 as output failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request line 23 as output (for oscillation)
    if (gpiod_line_request_output(line23, CONSUMER, 0) < 0) {
        perror("Request GPIO 23 as output failed");
        gpiod_chip_close(chip);
        return 1;
    }

    // Main loop: oscillate GPIO 23 at 10Hz, detect events on GPIO 17, and toggle GPIO 18
    while (1) {
        // Toggle GPIO 23 every 50 milliseconds (10Hz)
        toggle23 = !toggle23;
        gpiod_line_set_value(line23, toggle23);
        //printf("GPIO 23 set to: %d\n", toggle23);

        // Wait for an event on GPIO 17, with a timeout of 50 milliseconds
        int ret = gpiod_line_event_wait(line17, &ts);

        if (ret > 0) {  // Event detected on GPIO 17
            if (gpiod_line_event_read(line17, &event) == 0) {
                if (event.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
                   // printf("GPIO 17 rising edge detected\n");
                } else if (event.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
                   // printf("GPIO 17 falling edge detected\n");
                }

                // Toggle GPIO 18's state based on the detected event
                toggle18 = !toggle18;
                gpiod_line_set_value(line18, toggle18);
                //printf("GPIO 18 toggled to: %d\n", toggle18);
            }
        } else if (ret == 0) {  // Timeout, no event detected
            printf("No event detected on GPIO 17\n");
        } else {  // Error occurred
            perror("Error waiting for GPIO 17 event");
            break;
        }

        // Sleep for 50 milliseconds (to maintain the 10Hz oscillation for GPIO 23)
        usleep(100000);
    }

    // Clean up and release resources
    gpiod_line_release(line17);
    gpiod_line_release(line18);
    gpiod_line_release(line23);
    gpiod_chip_close(chip);

    return 0;
}
