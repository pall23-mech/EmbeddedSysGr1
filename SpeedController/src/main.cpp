// main.cpp
#include <Arduino.h>
#include "encoder.h"
#include "pwm_control.h" // Include the PWM control header

#define AIN2_PIN 8 // Pin for the motor direction control (Ain2)

// Create an encoder instance
Encoder encoder(16, 17, 1400.0);

// Define the variables to be shared with pwm_control.cpp
float targetPPS = 2000.0; // Desired speed (commanded reference speed)
float Kp = 4.5; // Proportional gain for the controller
unsigned long lastControlUpdate = 0; // Time of the last control update

void setup() {
    Serial.begin(9600); // Start serial communication at 9600 baud
    encoder.init();     // Initialize the encoder and interrupts

    setupPWM_Timer1(); // Set up Timer1 for PWM using the new function

    pinMode(AIN2_PIN, OUTPUT); // Set Ain2 as an output pin
    digitalWrite(AIN2_PIN, LOW); // LOW for one direction, change to HIGH for reverse direction
}

int main() {
    init(); // Initialize Arduino core libraries (including Serial)
    setup(); // Call the setup() function

    while (true) {
        controlLoop(); // Run the control loop (now part of the PWM module)
    }

    return 0;
}
