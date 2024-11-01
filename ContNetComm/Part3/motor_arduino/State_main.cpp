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

enum MotorState {
    STOPPED,
    RUNNING,
    STARTING,
    STATE_X // New state to handle specific custom behavior
};

MotorState motorState = STOPPED;

void setup() {
    // Initialize Serial and components
    Serial.begin(115200, SERIAL_8N1);
    timer.init(0.1); // Timer interval in ms
    sei();           // Enable global interrupts

    encoder.init();
    motorIN2.init();
    motorIN1.init(10);   // PWM frequency in ms
    motorIN1.set(0);     // Set initial duty cycle to 0 to keep motor off
    motorIN2.set_lo();   // Ensure motor is off
}

void loop() {
    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // Check for incoming messages only if there are enough bytes
    if (Serial.available() >= MSG_LEN) {
        Serial.readBytes(msg, MSG_LEN);

        // New command check to set state based on msg[4] table
        if (msg[0] == 0x00 && msg[1] == 0x06) { // First byte 0x00 and second byte 0x06
            switch (msg[4]) {
                case 0x01:
                    motorState = RUNNING;
                    Serial.println("Set node operational: RUNNING");
                    break;
                case 0x02:
                    motorState = STOPPED;
                    Serial.println("Stop node: STOPPED");
                    break;
                case 0x80:
                    motorState = STATE_X;
                    Serial.println("Set node pre-operational: STATE_X");
                    break;
                case 0x81:
                    motorState = STARTING;
                    Serial.println("Reset node: STARTING");
                    break;
                case 0x82:
                    motorState = STOPPED; // Example behavior for reset communication
                    refSpeed = 0;         // Reset any speed setting if necessary
                    Serial.println("Reset communications: STOPPED and reset");
                    break;
                default:
                    Serial.println("Unknown command in msg[4]");
                    break;
            }
        }

        // Existing motor control command checks
        else if (msg[0] == 0x01) { // Command for motor control
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
                
                // Check and update motor state based on targetSpeed
                if (targetSpeed > 0 && targetSpeed <= 65535) {
                    refSpeed = targetSpeed;
                    motorState = STARTING; // Set motor state to starting
                    Serial.println("Motor starting with target speed.");
                } else {
                    motorState = STOPPED; // Set motor state to stopped
                    Serial.println("Motor stopping.");
                }
            }
        }
    }

    // State machine to control motor based on motorState
    switch (motorState) {
        case STOPPED:
            motorIN2.set_lo();      // Disable motor
            motorIN1.set(0);        // Set PWM to 0 to stop motor
            refSpeed = 0;           // Reset target speed
            controlSignal = 0;
            break;

        case STARTING:
            motorIN2.set_hi();      // Enable motor
            motorState = RUNNING;   // Move to RUNNING state after setup
            break;

        case RUNNING:
            // Continuous control loop for updating motor speed
            actualSpeed = abs(encoder.speed());
            controlSignal = controller.update(refSpeed, actualSpeed);

            // Apply MIN_PWM_THRESHOLD only if refSpeed is non-zero
            if (refSpeed > 0) {
                controlSignal = constrain(controlSignal, MIN_PWM_THRESHOLD, 1.0);
            } else {
                controlSignal = 0;
            }

            pwmValue = controlSignal;
            motorIN1.set(pwmValue); // Continuously update motor PWM based on control signal
            break;

        case STATE_X:
            // Custom logic for STATE_X if needed
            motorIN2.set_hi();       // Ensure motor is enabled
            motorIN1.set(0.5);       // Example fixed PWM duty cycle
            Serial.println("Motor in STATE_X with custom behavior.");
            break;
    }

    // Periodically update encoder
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
