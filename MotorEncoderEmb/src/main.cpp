#include <Arduino.h>
#include "encoder.h"

// Initialize the encoder on A2 (PCINT10, pin 16) and A3 (PCINT11, pin 17)
Encoder encoder(16, 17);

void setup() 
{
    Serial.begin(9600);  // Start serial communication at 9600 baud
    encoder.init();      // Initialize the encoder and interrupts
}

void looping()
{
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    // Print the position at a regular interval (every 100 milliseconds)
    if (currentTime - lastPrintTime >= 100) 
    {
        Serial.print("Current Encoder Position: ");
        Serial.println(encoder.position());
        lastPrintTime = currentTime;
    }
}

int main()
{
    // Initialize Arduino core functions
    init();  // IMPORTANT: Initialize Arduino core libraries (including Serial)

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1) 
    {
        looping();  // Continuously check for updates and print position
    }

    return 0;
}
