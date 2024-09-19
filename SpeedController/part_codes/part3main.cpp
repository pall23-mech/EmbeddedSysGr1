// main.cpp
#include <Arduino.h>
#include "encoder.h"
#include "pwm_control.h" // Include the PWM control header
#include "analog_out.h"  // Include the Analog_out header

// Create an encoder instance
Encoder encoder(16, 17, 1400.0);

// Define the variables to be shared with pwm_control.cpp
float targetPPS = 2800.0; // Desired speed (commanded reference speed)
float Kp = 2.1; // Proportional gain for the controller
unsigned long lastControlUpdate = 0; // Time of the last control update

// Create an Analog_out instance directly for the direction control pin
Analog_out directionControl(8); // Pin number directly passed to the constructor

void setup() {
    Serial.begin(9600); // Start serial communication at 9600 baud
    encoder.init();     // Initialize the encoder and interrupts

    setupPWM_Timer1(); // Set up Timer1 for PWM using the new function

    // Use Analog_out to set up the direction pin
    directionControl.init(1); // Initialize with a dummy period since we're not using PWM here

    // Set the direction using Analog_out (0 for LOW, 1 for HIGH)
    directionControl.set(0); // Set to LOW initially for forward direction
}

int main() {
    init(); // Initialize Arduino core libraries (including Serial)
    setup(); // Call the setup() function

    while (true) {
        controlLoop(); // Run the control loop (now part of the PWM module)
    }

    return 0;
}
