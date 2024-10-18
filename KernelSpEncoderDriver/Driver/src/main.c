#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "pi_control.h"       // Include the PI controller header
#include "pi_pwm_control.h"   // Include the PWM control header

// Sysfs paths for encoder data
#define SYSFS_PATH "/sys/kernel/encoder/"
#define POSITION_FILE "position"
#define PPS_FILE "pps"
#define RPM_FILE "rpm"

// Function to read a value from a sysfs file
int read_sysfs_value(const char *file, int *value) {
    char path[256];
    FILE *fp;

    // Construct the full path to the sysfs file
    snprintf(path, sizeof(path), "%s%s", SYSFS_PATH, file);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening sysfs file");
        return -1;
    }

    // Read the integer value from the file
    if (fscanf(fp, "%d", value) != 1) {
        perror("Error reading value from sysfs file");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

// Function to read the encoder speed from sysfs
float read_encoder_speed() {
    int pps;
    if (read_sysfs_value(PPS_FILE, &pps) == 0) {
        return (float)pps;
    } else {
        printf("Warning: Unable to read PPS from sysfs.\n");
        return -1.0; // Return an invalid value to indicate an error
    }
}

// Main function
int main() {
    // Initialize the PI controller
    PIController controller;
    pi_controller_init(&controller, 1.0, 0.1, 200.0); // kp, ki, setpoint

    // Initialize PWM control
    pwm_init();

    // Time variables
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {
        // Measure elapsed time
        clock_gettime(CLOCK_MONOTONIC, &end);
        float elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        start = end;

        // Read the current speed from the encoder
        float current_speed = read_encoder_speed();

        // Ensure current_speed is valid, if not print a debug message
        if (current_speed < 0) {
            printf("Warning: Invalid encoder speed read: %.2f\n", current_speed);
            continue;
        }

        // Update the PI controller
        float control_signal = pi_controller_update(&controller, current_speed, elapsed_time);

        // Convert control signal to PWM value (scale to 0-255 for pigpio)
        int pwm_value = (int)(control_signal * 255 / 1023);
        if (pwm_value < 0) pwm_value = 0;
        if (pwm_value > 255) pwm_value = 255;

        // Set PWM based on control signal
        pwm_set(PWM_PIN1, pwm_value);
        pwm_set(PWM_PIN2, pwm_value);

        // Print debug information
        printf("Setpoint: %.2f, Current Speed: %.2f, PWM Value: %d\n",
               controller.setpoint, current_speed, pwm_value);

        // Delay to simulate loop timing
        usleep(100000); // 100 ms delay
    }

    // Cleanup
    pwm_cleanup();

    return 0;
}
