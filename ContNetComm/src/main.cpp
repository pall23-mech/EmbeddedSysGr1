#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include <Arduino.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"

Timer_msec timer;
Digital_out motorIN2(0);  // Motor direction control pin D8
Analog_out motorIN1(1);   // Motor PWM control pin D9
Encoder encoder(3, 4);    // Encoder pins D11, D12
float kp = 0.01;
float ti = 5;
PI_controller controller(kp, ti);

unsigned long lastPrintTime = 0;
double ref = 0;           // Target motor speed based on sensor value
double actual = 0;        // Actual motor speed from encoder
double pwmValue = 0;      // PWM duty cycle
double u = 0;             // Control signal from PI controller
char command[20];         // Buffer to hold incoming UART command
bool newCommand = false;  // Flag for new command

void setup() {  
    init(); // Initialize Arduino framework
    Serial.begin(9600, SERIAL_8N1);  
    timer.init(0.1);      // Initialize timer with 0.1 ms interval
    sei();                // Enable global interrupts

    encoder.init();
    motorIN2.init();
    motorIN1.init(10);    // Set PWM interval to 10 ms
    motorIN1.set(0);      // Start with zero duty cycle
    motorIN2.set_lo();    // Ensure motor is stopped initially

    // Initial Debug Message
    Serial.println("Motor Receiving Arduino Setup Complete.");
}

void loop() {
    // Check if data is available on UART
    if (Serial.available() > 0) {
        // Read the command until newline character '\n'
        int len = Serial.readBytesUntil('\n', command, sizeof(command) - 1);
        command[len] = '\0'; // Null-terminate the received string
        newCommand = true;
    }

    if (newCommand) {
        // Validate and parse the command if it starts with "MO:"
        if (strncmp(command, "MO:", 3) == 0) {
            int sensorValue = atoi(command + 3); // Extract value after "MO:"
            if (sensorValue >= 0 && sensorValue <= 120) { // Validate range
                // Map sensorValue from range 0-120 to 0-3500 RPM
                ref = map(sensorValue, 0, 120, 0, 3500); 
                
                // Debug information for received and mapped values
                Serial.print("Valid Command Received | Sensor Value: ");
                Serial.print(sensorValue);
                Serial.print(" -> Target Speed (RPM): ");
                Serial.println((int)ref);
            } else {
                // If out of expected range, print an error
                Serial.print("Error: Out of Range Sensor Value: ");
                Serial.println(sensorValue);
            }
        } else {
            // Print an error if the command format is unexpected
            Serial.print("Error: Unknown Command Format Received: ");
            Serial.println(command);
        }
        newCommand = false;
    }

    // Calculate control signal based on ref and actual motor speed
    actual = abs(encoder.speed());    // Get actual speed from encoder
    u = controller.update(ref, actual); // PI controller output
    u = constrain(u, 0.0, 0.999);     // Ensure PWM is within [0, 1]
    pwmValue = u;
    motorIN1.set(pwmValue);           // Update motor PWM with control signal

    // Periodic debug message for motor control values
    if (millis() - lastPrintTime >= 500) {
        Serial.print("Actual Speed (RPM): ");
        Serial.print(actual);
        Serial.print(" | PWM Duty Cycle: ");
        Serial.println(pwmValue);
        lastPrintTime = millis();
    }

    encoder.update();
    _delay_ms(3);                     // Small delay for stability
}

// Interrupt Service Routines for encoder updates and PWM control
ISR (INT0_vect) {
    encoder.update();
}

ISR(TIMER1_COMPA_vect) {
    motorIN1.pin.set_hi();      
}

ISR(TIMER1_COMPB_vect) {
    motorIN1.pin.set_lo(); 
}
