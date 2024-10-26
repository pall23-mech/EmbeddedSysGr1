// main.cpp
#include <Arduino.h>
#include "encoder.h"
#include "pwm_control.h" // Include the PWM control header
#include "analog_out.h"  // Include the Analog_out header

// Create an encoder instance
Encoder encoder(16, 17, 1400.0);

// Define the variables to be shared with pwm_control.cpp
float targetPPS = 2800.0; // Desired speed (commanded reference speed)
float Kp = 2.1;           // Proportional gain for the controller
unsigned long lastControlUpdate = 0; // Time of the last control update

// Create an Analog_out instance directly for the direction control pin
Analog_out directionControl(8); // Pin number directly passed to the constructor

// UART command buffer
char command[20];
bool newCommand = false; // Flag for new command

void setup() {
    Serial.begin(9600); // Start serial communication at 115200 baud
    encoder.init();       // Initialize the encoder and interrupts
    setupPWM_Timer1();    // Set up Timer1 for PWM using the new function

    // Use Analog_out to set up the direction pin
    directionControl.init(1); // Initialize with a dummy period since we're not using PWM here

    // Set the direction using Analog_out (0 for LOW, 1 for HIGH)
    directionControl.set(0); // Set to LOW initially for forward direction
}

void loop() {
    // Check for UART data
    if (Serial.available() > 0) {
        int len = Serial.readBytesUntil('\n', command, sizeof(command) - 1);
        command[len] = '\0'; // Null-terminate the received string
        newCommand = true;
    }

    // Process the command if a new one is available
    if (newCommand) {
        // Check for the "MO:" prefix and parse the value
        if (strncmp(command, "MO:", 3) == 0) {
            int sensorValue = atoi(command + 3); // Extract value after "MO:"
            if (sensorValue >= 0 && sensorValue <= 120) { // Validate range
                // Map sensorValue from range 0-120 to 0-3500 PPS
                targetPPS = map(sensorValue, 0, 120, 0, 3500); 

                // Debug information for received and mapped values
                Serial.print("Valid Command Received | Sensor Value: ");
                Serial.print(sensorValue);
                Serial.print(" -> Target Speed (PPS): ");
                Serial.println((int)targetPPS);
            } else {
                // Print an error for out-of-range values
                Serial.print("Error: Out of Range Sensor Value: ");
                Serial.println(sensorValue);
            }
        } else {
            // Print an error if the command format is unexpected
            Serial.print("Error: Unknown Command Format Received: ");
            Serial.println(command);
        }
        newCommand = false;
    }

    // Continuously run the control loop
    controlLoop(); // Adjust motor speed according to the targetPPS and encoder feedback
}

int main() {
    init(); // Initialize Arduino core libraries (including Serial)
    setup(); // Call the setup() function

    while (true) {
        loop(); // Continuously run the loop to check UART and control motor
    }

    return 0;
}
