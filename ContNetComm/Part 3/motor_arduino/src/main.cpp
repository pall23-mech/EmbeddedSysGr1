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
PI_controller controller(kp, ti);

unsigned long lastPrintTime = 0;
double actualSpeed = 0;
double refSpeed = 0;
double pwmValue = 0;
double controlSignal = 0;

void setup() {
    // Initialize Serial and components
    Serial.begin(115200, SERIAL_8N1);
    timer.init(0.1); // Timer interval in ms
    sei();           // Enable global interrupts

    encoder.init();
    motorIN2.init();
    motorIN1.init(10);   // PWM frequency in ms
    motorIN1.set(0);     // Set initial duty cycle to 0
    motorIN2.set_lo();   // Ensure motor is off

    Serial.println("Motor control setup complete.");
}

void loop() {
    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // Check for incoming messages only if there are enough bytes
    if (Serial.available() >= MSG_LEN) {
        Serial.readBytes(msg, MSG_LEN);

        // Check if this command is intended for motor control
        if (msg[0] == 0x01) {
            if (msg[1] == 0x03) { // Request to read motor speed
                actualSpeed = abs(encoder.speed()); // Get motor speed
                uint16_t speedData = (uint16_t)actualSpeed;
                
                // Prepare message response with motor speed
                msg[4] = (uint8_t)(speedData >> 8);   // High byte
                msg[5] = (uint8_t)(speedData & 0xFF); // Low byte
                msg[6] = 0xFF; // Placeholder for CRC (if not used)
                msg[7] = 0xFF; // Placeholder for CRC (if not used)
                
                Serial.write(msg, MSG_LEN); // Send motor speed back to RPi
            }
            if (msg[1] == 0x06) { // Command to set motor speed
                uint16_t targetSpeed = (msg[4] << 8) | msg[5];
                if (targetSpeed >= 0 && targetSpeed <= 255) { // Valid range check
                    refSpeed = targetSpeed;
                    actualSpeed = abs(encoder.speed());
                    controlSignal = controller.update(refSpeed, actualSpeed);
                    controlSignal = constrain(controlSignal, 0.0, 1.0); // Constrain to PWM range
                    pwmValue = controlSignal;
                    motorIN1.set(pwmValue); // Update motor PWM based on control signal

                    // Acknowledge the command
                    Serial.println("Motor speed command received and executed.");
                }
            }
        }
    }

    // Periodically update encoder and control loop
    encoder.update();
    _delay_ms(3);
}

// Interrupt Service Routine for Encoder Update
ISR(INT0_vect) {
    encoder.update();
}

// ISR for PWM control (if needed for precise timing)
ISR(TIMER1_COMPA_vect) {
    motorIN1.pin.set_hi();
}

ISR(TIMER1_COMPB_vect) {
    motorIN1.pin.set_lo();
}
