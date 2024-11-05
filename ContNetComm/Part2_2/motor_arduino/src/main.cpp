#include <Arduino.h>
#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"

Timer_msec timer;
Digital_out motorIN2(0); // pin D8 for motor control
Analog_out motorIN1(1);  // PWM pin D9 for motor control
Encoder encoder(3, 4);   // Encoder pins D11, D12
float kp = 0.01;
float ti = 5;
float MIN_PWM_THRESHOLD = 0.1;
PI_controller controller(kp, ti);

unsigned long lastPrintTime = 0;
double actualSpeed = 0;
double refSpeed = 0; // Set initial target speed to 0 to keep motor off initially
double pwmValue = 0;
double controlSignal = 0;

uint16_t ModRTU_CRC(uint8_t buf[], int len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc
        for (int i = 8; i != 0; i--) { // Loop over each bit
            if ((crc & 0x0001) != 0) { // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            } else {
                crc >>= 1; // Just shift right
            }
        }
    }
    return crc;
}

void setup() {
    // Initialize Serial and components
    Serial.begin(115200, SERIAL_8N1);
    timer.init(0.1); // Timer interval in ms
    sei();           // Enable global interrupts

    encoder.init();
    motorIN2.init();
    motorIN1.init(10);   // PWM frequency in ms
    motorIN1.set(0);     // Set initial PWM duty cycle to 0 to keep motor off
    motorIN2.set_lo();   // Ensure motor is off
}

void loop() {
    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // Check for incoming messages only if there are enough bytes
    if (Serial.available() >= MSG_LEN) {
        Serial.readBytes(msg, MSG_LEN);

        // Compute CRC for verification
        uint16_t receivedCRC = (msg[6] << 8) | msg[7]; // CRC from the message
        uint16_t calculatedCRC = ModRTU_CRC(msg, 6);    // CRC calculated from the first 6 bytes

        // Verify CRC before processing the command
        if (receivedCRC == calculatedCRC) {
            // Check if this command is intended for motor control
            if (msg[0] == 0x01) {
                if (msg[1] == 0x03) { // Request to read motor speed
                    actualSpeed = abs(encoder.speed()); // Get motor speed
                    uint16_t speedData = (uint16_t)actualSpeed;
                    
                    // Prepare message response with motor speed
                    msg[4] = (uint8_t)(speedData >> 8);   // High byte
                    msg[5] = (uint8_t)(speedData & 0xFF); // Low byte
                    
                    // Compute and attach CRC to response
                    uint16_t responseCRC = ModRTU_CRC(msg, 6);
                    msg[6] = (uint8_t)(responseCRC >> 8);
                    msg[7] = (uint8_t)(responseCRC & 0xFF);
                    
                    Serial.write(msg, MSG_LEN); // Send motor speed back to RPi
                }
                if (msg[1] == 0x06) { // Command to set motor speed
                    uint16_t targetSpeed = (msg[4] << 8) | msg[5];
                    
                    // Adjust the valid range check to accept higher speeds
                    if (targetSpeed >= 0 && targetSpeed <= 65535) { // Adjusted range check
                        refSpeed = targetSpeed;
                        motorIN2.set_hi(); // Turn on motor only when a speed command is received

                        // Acknowledge the command with CRC
                        uint16_t responseCRC = ModRTU_CRC(msg, 6);
                        msg[6] = (uint8_t)(responseCRC >> 8);
                        msg[7] = (uint8_t)(responseCRC & 0xFF);
                        Serial.write(msg, MSG_LEN); // Send acknowledgment back to RPi
                    } else {
                        Serial.println("Invalid speed range"); // For debugging
                    }
                }
            }
        } else {
            Serial.println("CRC check failed"); // Indicate CRC failure for debugging
        }
    }

    // Continuous control loop for updating motor speed
    actualSpeed = abs(encoder.speed());
    controlSignal = controller.update(refSpeed, actualSpeed);

    // Apply MIN_PWM_THRESHOLD only if refSpeed is non-zero
    if (refSpeed > 0) {
        controlSignal = constrain(controlSignal, MIN_PWM_THRESHOLD, 1.0); // Constrain with threshold if motor should run
    } else {
        controlSignal = 0; // Ensure motor is off if refSpeed is zero
    }

    pwmValue = controlSignal;
    motorIN1.set(pwmValue); // Continuously update motor PWM based on control signal

    // Periodically update encoder and control loop
    encoder.update();
    _delay_ms(3);
}
