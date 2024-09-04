#include <Arduino.h>
#include "encoder.h"

Encoder encoder(3, 4); // C1 on pin 3 (INT1), C2 on pin 4

void setup() 
{
    Serial.begin(9600);
    encoder.init(); // Initialize the encoder
}

void looping()
{
    // The update function is not needed anymore, as interrupts handle the encoder updates

    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    // Print the position at a regular interval (e.g., every 100 milliseconds)
    if (currentTime - lastPrintTime >= 100) 
    {
        Serial.print("Current Encoder Position: ");
        Serial.println(encoder.position());
        lastPrintTime = currentTime;
    }

    delayMicroseconds(71); // Add a small delay between checks
}

int main()
{
    // Initialize Arduino core functions
    init();

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1) 
    {
        looping();
    }

    return 0;
}
