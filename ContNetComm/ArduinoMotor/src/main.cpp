#include <Arduino.h>
#include "encoder.h"
//tau 1: 726ms, tau 2: 834ms tau 3: 876ms
// Initialize the encoder on A2 (PCINT10, pin 16) and A3 (PCINT11, pin 17), 1400 pulses per rev
Encoder encoder(16, 17, 1400.0);
int null_postion = 0;  // Track the state of the encoder (whether it's in the initial state, moving, or done)
unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long elapsedTime = 0;
float maxPPS = 3490;  // Pre-measured maximum PPS
float mesMaxPPS = 0;  // Maximum PPS measured in this run
float omega1 = 0.63 * maxPPS;  // Threshold for tau measurement (63% of maxPPS)
float mesTau = 0;  // Measured tau (to be printed)
float speedPPS;

void setup() {
    Serial.begin(9600);  // Start serial communication at 9600 baud
    encoder.init();      // Initialize the encoder and interrupts
}

void looping() {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();
    
    // Detect the first movement of the motor (start recording time)
    if (encoder.position() != null_postion && null_postion == 0) {
        startTime = currentTime;  // Record the start time when the motor first moves
        null_postion = 1;         // Update state to indicate movement has started
    }

    // Check if the speed reaches 63% of maxPPS and record the time (for tau)
    if (encoder.speedPPS() >= omega1 && null_postion == 1) {
        endTime = currentTime;  // Record the time when speed hits 63% of maxPPS
        elapsedTime = endTime - startTime;
        mesTau = elapsedTime;  // Store the measured tau value
        null_postion = 2;      // Update state to indicate tau measurement is done
    }

    // Print the position and speed every second
    if (currentTime - lastPrintTime >= 1000) {
        encoder.updateSpeed();  // Update the speed (PPS and RPM)

        // Update mesMaxPPS if the current speed is the highest recorded so far
        speedPPS = encoder.speedPPS();
        if (speedPPS >= mesMaxPPS) {
            mesMaxPPS = speedPPS;  // Track the highest speed measured
        }

        // Print position, speed, and other values
        Serial.print("Position: ");
        Serial.println(encoder.position());

        Serial.print("Speed (PPS): ");
        Serial.println(speedPPS);

        Serial.print("Speed (RPM): ");
        Serial.println(encoder.speedRPM());

        // Print the maximum measured PPS in this run
        Serial.print("Maximum measured (PPS): ");
        Serial.println(mesMaxPPS);

        // Print the measured tau when it's available (after tau has been calculated)
        
        Serial.print("Measured tau [ms]: ");
        Serial.println(mesTau);
        

        lastPrintTime = currentTime;  // Update the time of the last print
    }
}

int main() {
    // Initialize Arduino core functions
    init();  // Initialize Arduino core libraries (including Serial)

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1) {
        looping();  // Continuously check for updates and print position
    }

    return 0;
}
