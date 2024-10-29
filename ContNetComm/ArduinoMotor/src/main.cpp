#include <Arduino.h>
#include "encoder.h"
#include "pwm_control.h" // Include the PWM control header
#include "analog_out.h"  // Include the Analog_out header

// Create an encoder instance
Encoder encoder(2, 3, 1400.0);

// Define the variables to be shared with pwm_control.cpp
float targetPPS = 2800.0; // Desired speed (commanded reference speed)
float Kp = 2.1; // Proportional gain for the controller
unsigned long lastControlUpdate = 0; // Time of the last control update

// Create an Analog_out instance directly for the direction control pin
Analog_out directionControl(8); // Pin number directly passed to the constructor

void setup() {
    Serial.begin(9600); // Start serial communication at 9600 baud to match text_com.c
    encoder.init();     // Initialize the encoder and interrupts

    setupPWM_Timer1(); // Set up Timer1 for PWM using the new function

    // Use Analog_out to set up the direction pin
    directionControl.init(1); // Initialize with a dummy period since we're not using PWM here

    // Set the direction using Analog_out (0 for LOW, 1 for HIGH)
    directionControl.set(0); // Set to LOW initially for forward direction
}

void sendResponse(String prefix, float value) {
    char buffer[100];
    sprintf(buffer, "%s :%.2f\\n", prefix.c_str(), static_cast<double>(value)); // Cast float to double
    Serial.print(buffer); // Send the formatted response
}

int main() {
    init(); // Initialize Arduino core libraries (including Serial)
    setup(); // Call the setup() function

    while (true) {
        controlLoop(); // Run the control loop (now part of the PWM module)
        
        // Listen for commands from the Raspberry Pi
        if (Serial.available() > 0) {
            String command = Serial.readStringUntil('\0'); // Corrected null character notation

            if (command == "MO") { // Command for motor control
                sendResponse("MO", encoder.speedPPS()); // Respond with motor speed (PPS)
            } else if (command == "SE") { // Command for sensor data
                sendResponse("SE", encoder.position()); // Respond with encoder position as sensor data
            } else {
                Serial.println("Unknown command"); // Default response for unrecognized command
            }
        }
    }

    return 0;
}
