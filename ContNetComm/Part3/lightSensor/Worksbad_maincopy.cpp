#include <Arduino.h>

unsigned long lastSendTime = 0;
int analogPin = A3; // Sensor input pin
float smoothedValue = 0.0;
float alpha = 0.1;  // Adjust smoothing sensitivity
char buffer[100];   // Increased buffer size for response formatting

// Initialize ModRTU_CRC function for CRC checks
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
    Serial.begin(115200, SERIAL_8N1); // Initialize UART for command transmission
    pinMode(analogPin, INPUT);
}

void loop() {
    // Apply exponential smoothing to reduce sensor noise
    int rawSensorValue = analogRead(analogPin);
    smoothedValue = alpha * (rawSensorValue - smoothedValue) + smoothedValue;

    // Map the smoothed value to a specific range if needed
    int mappedValue = map((int)smoothedValue, 0, 1023, 0, 120);

    // Prepare data for transmission, including CRC
    uint8_t dataToSend[3] = { (uint8_t)mappedValue, (uint8_t)(mappedValue >> 8), 0 };
    uint16_t crc = ModRTU_CRC(dataToSend, 2);
    dataToSend[2] = crc & 0xFF;
    dataToSend[3] = (crc >> 8) & 0xFF;

    // Print data to Serial, following the same protocol format
    sprintf(buffer, "Data: %d | CRC: 0x%04X", mappedValue, crc);
    Serial.println(buffer);

    // Control data send rate
    delay(1000);
}

