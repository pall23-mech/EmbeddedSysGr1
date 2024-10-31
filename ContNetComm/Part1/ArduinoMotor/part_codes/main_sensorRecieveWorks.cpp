#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include <Arduino.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"

Timer_msec timer;
Digital_out motorIN2(0);  // Motor control pin D8
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
        // Parse the command if it starts with "MO:"
        if (strncmp(command, "MO:", 3) == 0) {
            ref = atoi(command + 3); // Convert the number after "MO:" to integer
            Serial.print("MO :Reference set to ");
            Serial.println((int)ref);
        } else {
            Serial.print("MO :Unknown command: ");
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
