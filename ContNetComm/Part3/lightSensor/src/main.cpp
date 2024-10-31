#include <Arduino.h>

unsigned long lastSendTime = 0;
int analogPin = A3; // Sensor input pin
float smoothedValue = 0.0;
float alpha = 0.1;  // Adjust smoothing sensitivity
char buffer[100];   // Increased buffer size for response formatting

// Removed ModRTU_CRC function since CRC checks will be skipped

void setup() {
    Serial.begin(115200, SERIAL_8N1); // Initialize UART for communication
    pinMode(analogPin, INPUT);
}

void loop() {
    // Apply exponential smoothing to reduce sensor noise
    int rawSensorValue = analogRead(analogPin);
    smoothedValue = alpha * (rawSensorValue - smoothedValue) + smoothedValue;
    uint16_t data = (uint16_t)smoothedValue;

    // Check if data is available to read from Serial
    if (Serial.available() > 0) { // bytes received
        const size_t MSG_LEN = 8;
        uint8_t msg[MSG_LEN];
        Serial.readBytes(msg, MSG_LEN);

        if (msg[0] == 2) {
            if (msg[1] == 0x03) {    // Am I reading?
                //Serial.write("ABCDEFGH");
                if (msg[3] == 0) {   // Am I reading from the sensor?
                    //Serial.write("AAAAAAAA");
                    _delay_ms(100);
                    if (msg[5] == 1) { // RPi only asked for one value?
                        //Serial.print("HGFEDCBA");
                        data = (uint16_t)(smoothedValue);
                        msg[4] = (uint8_t)(data >> 8);
                        msg[5] = (uint8_t)(data & 0x00FF);
                        msg[6] = (uint8_t)(0x00FF);
                        msg[7] = (uint8_t)(0x00FF);

                        // Skipping the CRC calculation and check here
                        Serial.write(msg, 8); // Success, sending the message back to the RPi
                    }
                }
            }
            if (msg[1] == 0x06) { // Am I writing?
                Serial.write("Helloshe"); // Custom response for write request
            }
        }
    }

    // Control data send rate to avoid overloading the loop
    delay(100);
}
