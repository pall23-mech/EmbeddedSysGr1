#include "encoder.h"
#include <Arduino.h>

Encoder::Encoder(int pin_A, int pin_B)
    : encoder_pin_A(pin_A), encoder_pin_B(pin_B), count(0), last_state_A(false), last_state_B(false)
{
    // Encoder constructor body
}

void Encoder::init() {
    encoder_pin_A.init();  // Initialize encoder pin A (C1) as input
    encoder_pin_B.init();  // Initialize encoder pin B (C2) as input
    last_state_A = encoder_pin_A.is_lo();  // Capture the initial state of encoder pin A
    last_state_B = encoder_pin_B.is_lo();  // Capture the initial state of encoder pin B

    DDRB |= (1 << PB5); // Set PB5 (LED) as output for signaling state change
}

void Encoder::update() {
    bool current_state_A = encoder_pin_A.is_lo();  // Read the current state of encoder pin A (C1)
    bool current_state_B = encoder_pin_B.is_lo();  // Read the current state of encoder pin B (C2)

    // Check if the state of C1 has changed
    if (current_state_A != last_state_A) {
        // Determine direction based on C2
        if (current_state_B != current_state_A) {
            count++;  // Increment if C1 leads C2
        } else {
            count--;  // Decrement if C2 leads C1
        }

        // Signal that the state change has been handled
        PORTB ^= (1 << PB5); // Turn on LED  //nefna sem object og led.toggle
        //_delay_us(10);       // DO NOT CHANGE THIS DELAY
        //PORTB &= ~(1 << PB5); // Turn off LED

        // Update the last states
        last_state_A = current_state_A;
        last_state_B = current_state_B;
    }
}

int Encoder::position() const {
    return count;  // Return the current position count
}
