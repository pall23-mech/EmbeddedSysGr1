#include <Arduino.h>
#include "encoder.h"

// Initialize the encoder on A2 (PCINT10, pin 16) and A3 (PCINT11, pin 17) and 1400 pulses per rev, from the specifications
Encoder encoder(16, 17, 1400.0);

float highest_RPM = 0;
float highest_PPS = 0;
unsigned long startTime = 0;
const unsigned long duration = 10000; // Run for 10 seconds
const unsigned long interval = 100; // Sample RPM every 100 ms

void setup()
{
    Serial.begin(9600); // Start serial communication at 9600 baud
    encoder.init();     // Initialize the encoder and interrupts
}

void looping()
{
    static unsigned long lastSampleTime = 0;
    unsigned long currentTime = millis();

    // Print the position at a regular interval (every second)
    if (currentTime - startTime >= duration)
    {   
        Serial.println("This is the results after running for 10s \n");
        Serial.print("Highest RPM: ");
        Serial.println(highest_RPM);
        Serial.print("Highest PPS: ");
        Serial.println(highest_PPS);
        while(1);
    }

    if (currentTime - lastSampleTime >= interval)
    {
        encoder.updateSpeed(); // Update the speed (PPS and RPM) based on the current position and time
        float rpm = encoder.speedRPM();
        float pps = encoder.speedPPS();

        if (rpm > highest_RPM)
        {
            highest_RPM = rpm;

        }

        if (pps > highest_PPS)
        {
            highest_PPS = pps;
            
        }

        lastSampleTime = currentTime;
    }
}

int main()
{
    // Initialize Arduino core functions
    init(); // IMPORTANT: Initialize Arduino core libraries (including Serial)

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1)
    {
        looping(); // Continuously check for updates and print position
    }

    return 0;
}
