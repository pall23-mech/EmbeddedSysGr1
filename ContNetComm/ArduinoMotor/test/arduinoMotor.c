#include <Arduino.h>

int motorPin = 11;  // Pin connected to the motor control (PWM pin)

void setup() {
   Serial.begin(115200);  // Initialize Serial at 115200 baud rate
   pinMode(motorPin, OUTPUT);  // Set motor pin as output
}

void loop() {
   String command;
   char buffer[100];

   if (Serial.available() > 0) {  // Check if data is available
      command = Serial.readStringUntil('\n');  // Read until newline

      if (command.startsWith("M ")) {  // Check for "M " command for motor
         int level = command.substring(2).toInt();  // Extract motor control value
         if (level >= 0 && level <= 255) {
            analogWrite(motorPin, level);  // Set motor speed
            sprintf(buffer, "Set motor speed to %d\n", level);
         } else {
            sprintf(buffer, "Error: %d is out of range\n", level);
         }
      } else {
         sprintf(buffer, "Unknown command: %s\n", command.c_str());
      }
      Serial.println(buffer);  // Send response to Raspberry Pi
   }
}