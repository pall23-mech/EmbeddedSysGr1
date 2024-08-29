#include <Arduino.h>
#include "encoder.h"

Encoder encoder(3, 4); // C1 on PB3 (Digital Pin 11), C2 on PB4 (Digital Pin 12)

void setup() {
    Serial.begin(9600);
    encoder.init(); // Initialize the encoder
}

void loop() {
    encoder.update();    // Continuously update the encoder count

    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    // Print the position at a regular interval (e.g., every 100 milliseconds)
    if (currentTime - lastPrintTime >= 100) {
        Serial.print("Current Encoder Position: ");
        Serial.println(encoder.position());
        lastPrintTime = currentTime;
    }

    delayMicroseconds(140); // Add a 140-microsecond delay between checks (not exact, since delay() is in milliseconds)
}
