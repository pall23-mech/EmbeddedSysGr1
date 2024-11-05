#include <Arduino.h>

unsigned long lastSendTime = 0;
int analogPin = A3; // Sensor input pin
int sensorValue = 0;
char buffer[100]; // Increased buffer size for consistent response formatting

void setup() {
    Serial.begin(115200, SERIAL_8N1); // Initialize UART for command transmission
    pinMode(analogPin, INPUT);
}

void loop() {
    // Read the raw sensor value and map it
    int rawSensorValue = analogRead(analogPin);
    sensorValue = map(rawSensorValue, 0, 1023, 0, 120);

    // Check for an incoming command from the Raspberry Pi
    if (Serial.available()) {
        String command = Serial.readStringUntil('\0'); // Read command until null character
        
        if (command == "SE") {
            // Format the message to send the sensor value with "SE" prefix
            snprintf(buffer, sizeof(buffer), "SE :Sensor value %d\n", sensorValue);
            Serial.print(buffer); // Send the response
            
        } else {
            // Send a default response for unknown commands
            Serial.println("Unknown command");
        }
    }

    // Delay to stabilize loop speed
    delay(100); 
}
