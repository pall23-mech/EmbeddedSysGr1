#include "encoder.h"
#include <Arduino.h>

// Global volatile variable to keep track of the encoder count
volatile int encoderPosition = 0;  // Position of the encoder

// Static variables for the encoder pins (interrupts need them to be global)
static int encoderPinA;
static int encoderPinB;

// Pin Change Interrupt Service Routine
ISR(PCINT1_vect) {
    // Read the current state of both encoder pins
    bool state_A = digitalRead(encoderPinA);
    bool state_B = digitalRead(encoderPinB);

    // Determine direction based on the states of A and B
    if (state_A != state_B) {
        encoderPosition++;  // Clockwise rotation
    } else {
        encoderPosition--;  // Counterclockwise rotation
    }
}

Encoder::Encoder(int pin_A, int pin_B)
    : encoder_pin_A(pin_A), encoder_pin_B(pin_B), count(0) {
    // Constructor body
}

void Encoder::init() {
    // Store the pins in static variables for ISR access
    encoderPinA = encoder_pin_A;
    encoderPinB = encoder_pin_B;

    // Initialize pins as inputs
    pinMode(encoder_pin_A, INPUT);
    pinMode(encoder_pin_B, INPUT);

    // Enable Pin Change Interrupts for A2 (PCINT10) and A3 (PCINT11)
    PCICR |= (1 << PCIE1); // Enable Pin Change Interrupts for PCINT[14:8] (which includes A2 and A3)
   // PCMSK1 |= (1 << PCINT10); // Enable interrupt for A2 (PCINT10)
    PCMSK1 |= (1 << PCINT11); // Enable interrupt for A3 (PCINT11)
}

int Encoder::position() const {
    return encoderPosition;  // Return the current position count
}
