#include <pigpio.h>
#include <stdio.h>

#define PWM_PIN1 12 // GPIO12
#define PWM_PIN2 13 // GPIO13

// Initialize PWM on specified pins
// Example implementation
void pwm_init(void) {
    if (gpioInitialise() < 0) {
        printf("Pigpio initialization failed\n");
    }
}


    // Set PWM range and frequency for each pin (adjust frequency as needed)
    gpioSetMode(PWM_PIN1, PI_OUTPUT);
    gpioSetMode(PWM_PIN2, PI_OUTPUT);
    gpioSetPWMrange(PWM_PIN1, 255);
    gpioSetPWMrange(PWM_PIN2, 255);
    gpioSetPWMfrequency(PWM_PIN1, 1000); // 1 kHz frequency
    gpioSetPWMfrequency(PWM_PIN2, 1000); // 1 kHz frequency
}

// Set PWM value for motor control (value range: 0-255)
void pwm_set(int pin, int value) {
    if (value < 0) value = 0;
    if (value > 255) value = 255;
    gpioPWM(pin, value);
}

// Cleanup function
void pwm_cleanup() {
    gpioTerminate();
}
