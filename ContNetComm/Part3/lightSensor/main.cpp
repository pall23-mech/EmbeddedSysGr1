#include <Arduino.h>

unsigned long lastSendTime = 0;
int analogPin = A3; // Sensor input pin
float smoothedValue = 0.0;
float alpha = 0.1;  // Adjust smoothing sensitivity
char buffer[100];   // Increased buffer size for response formatting

// Function to calculate Modbus RTU CRC
uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos];
        for (int i = 8; i != 0; i--) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void setup() {
    Serial.begin(115200, SERIAL_8N1); // Initialize UART for communication
    pinMode(analogPin, INPUT);
    Serial.println("Arduino Modbus Interface with CRC"); // Debug message
}

void loop() {
    // Apply exponential smoothing to reduce sensor noise
    int rawSensorValue = analogRead(analogPin);
    smoothedValue = alpha * (rawSensorValue - smoothedValue) + smoothedValue;
    uint16_t data = (uint16_t)smoothedValue;

    // Check if data is available to read from Serial
    if (Serial.available() >= 8) { // Expecting at least 8 bytes
        const size_t MSG_LEN = 8;
        uint8_t msg[MSG_LEN];
        Serial.readBytes(msg, MSG_LEN);

        // Verify device ID
        if (msg[0] == 2) {
            uint16_t receivedCRC = (msg[6] << 8) | msg[7];
            uint16_t calculatedCRC = ModRTU_CRC(msg, 6); // CRC over the first 6 bytes

            if (receivedCRC == calculatedCRC) { // CRC is valid
                if (msg[1] == 0x03 && msg[3] == 0 && msg[5] == 1) { // Read command and single value request
                    // Prepare response with sensor data
                    msg[4] = (uint8_t)(data >> 8);    // High byte of sensor data
                    msg[5] = (uint8_t)(data & 0x00FF); // Low byte of sensor data
                    
                    // Calculate CRC for response
                    calculatedCRC = ModRTU_CRC(msg, 6);
                    msg[6] = (uint8_t)(calculatedCRC >> 8);
                    msg[7] = (uint8_t)(calculatedCRC & 0xFF);

                    // Send the response with data and CRC
                    Serial.write(msg, MSG_LEN);
                }
                if (msg[1] == 0x06) { // Write command
                    Serial.write("Helloshe"); // Example response for a write request
                }
            } else {
                Serial.println("CRC mismatch detected"); // Debug message for CRC failure
            }
        } else {
            Serial.println("Unknown Device ID"); // Debug message for incorrect device ID
        }
    }

    // Control data send rate to avoid overloading the loop
    delay(100);
}
