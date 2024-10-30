#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include <Arduino.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"

// #include "P_controller.h"

Timer_msec timer;
Digital_out motorIN2(0); // pin D8
Analog_out motorIN1(1);  // PWM pin D9
Encoder encoder(3, 4);   // encoder pin D11 D12
float kp = 0.01;
float ti = 5;
PI_controller controller(kp, ti);
// P_controller controller(kp);

unsigned long lastPrintTime = 0;
int analogPin = A3;
double ref = 0;
double actual = 0;
double pwmValue = 0;
double u = 0;

void setup() {
    // Initialize Arduino framework
    Serial.begin(115200);
    timer.init(0.1); // Timer interval in ms
    sei();           // Enable global interrupts

    // Initialize components
    encoder.init();
    motorIN2.init();
    motorIN1.init(10);  // PWM frequency in ms
    motorIN1.set(0);    // Set initial duty cycle to 0
    motorIN2.set_lo();  // Ensure motor is off

    Serial.println("Setup complete.");
}

void loop() {
    const size_t MSG_LEN = 8;
    uint8_t msg[MSG_LEN];

    // Check for available data and read only if 8 bytes are available
    if (Serial.available() >= MSG_LEN) {
        Serial.readBytes(msg, MSG_LEN);

        // Check if the command is for this Arduino
        if (msg[0] == 0x01) {
            if (msg[1] == 0x03) {
                // Implement read functionality if needed
            }
            if (msg[1] == 0x06) { // Writing command
                uint16_t reg = (msg[2] << 8 | msg[3]);
                
                if (reg == 0x00) { // Motor control register
                    uint16_t refVal = (msg[4] << 8 | msg[5]);
                    
                    if (refVal >= 0 && refVal <= 255) { // Range check
                        actual = abs(encoder.speed());
                        u = controller.update(refVal, actual);
                        u = constrain(u, 0.0, 1.0); // Constrain within full PWM range
                        pwmValue = u;
                        motorIN1.set(pwmValue);

                        // Acknowledge by sending back the message to the RPi
                        Serial.println("Command received and executed:");
                        for (int i = 0; i < MSG_LEN; i++) {
                            Serial.print("0x");
                            Serial.print(msg[i], HEX);
                            Serial.print(" ");
                        }
                        Serial.println();
                    }
                }
            }
        }
    }

    // Update encoder and delay for consistent operation
    encoder.update();
    _delay_ms(3);
}

// Interrupt Service Routine for Encoder Update
ISR(INT0_vect) {
    encoder.update();
}

// Interrupt Service Routine for Timer1 Compare A
ISR(TIMER1_COMPA_vect) {
    motorIN1.pin.set_hi();
}

// Interrupt Service Routine for Timer1 Compare B
ISR(TIMER1_COMPB_vect) {
    motorIN1.pin.set_lo();
}
