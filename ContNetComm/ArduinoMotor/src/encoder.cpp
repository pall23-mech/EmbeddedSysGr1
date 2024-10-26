#include "encoder.h"
#include <Arduino.h>

// Global volatile variable to keep track of the encoder count
volatile int encoderPosition = 0; // Position of the encoder

// Static variables for the encoder pins
static int encoderPinA;
static int encoderPinB;
bool last_stateA = false;
bool last_stateB = false;
static int pulses_per_rev;

// Interrupt Service Routine for Channel A
void ISR_A() {
    // Read the current state of both encoder pins
    bool state_A = digitalRead(encoderPinA);
    bool state_B = digitalRead(encoderPinB);

    // Determine direction based on the states of A and B
    if (state_A != last_stateA) {
        if (state_B != state_A) {
            encoderPosition++; // Clockwise rotation
        } else {
            encoderPosition--; // Counterclockwise rotation
        }
    }
    last_stateA = state_A; // Update last state for A
}

// Interrupt Service Routine for Channel B
void ISR_B() {
    // Read the current state of both encoder pins
    bool state_A = digitalRead(encoderPinA);
    bool state_B = digitalRead(encoderPinB);

    // Determine direction based on the states of A and B
    if (state_B != last_stateB) {
        if (state_A == state_B) {
            encoderPosition++; // Clockwise rotation
        } else {
            encoderPosition--; // Counterclockwise rotation
        }
    }
    last_stateB = state_B; // Update last state for B
}

// Encoder class constructor
Encoder::Encoder(int pin_A, int pin_B, int ppr)
    : encoder_pin_A(pin_A), encoder_pin_B(pin_B), pulses_per_rev(ppr), count(0), last_position(0), last_time(0) {
    // Empty constructor body
}

void Encoder::init() {
    // Store the pins in static variables for ISR access
    encoderPinA = encoder_pin_A; // D2 (INT0)
    encoderPinB = encoder_pin_B; // D3 (INT1)

    // Initialize pins as inputs
    pinMode(encoder_pin_A, INPUT);
    pinMode(encoder_pin_B, INPUT);

    // Attach interrupts to D2 (INT0) and D3 (INT1)
    attachInterrupt(digitalPinToInterrupt(encoderPinA), ISR_A, CHANGE); // D2 interrupt
    attachInterrupt(digitalPinToInterrupt(encoderPinB), ISR_B, CHANGE); // D3 interrupt

    last_time = millis(); // Initialize the last time
}

int Encoder::position() const {
    return encoderPosition; // Return the current position count
}

void Encoder::updateSpeed() {
    unsigned long currentTime = millis();
    unsigned long deltaTime = currentTime - last_time;

    if (deltaTime > 0) {
        int deltaPosition = encoderPosition - last_position;
        pps = (float)deltaPosition / (deltaTime / 1000.0); // Calculate pulses per second
        rpm = (pps * 60.0) / pulses_per_rev;

        // Update last position and time
        last_position = encoderPosition;
        last_time = currentTime;
    }
}

float Encoder::speedPPS() const {
    return pps; // Return the last stored PPS value
}

float Encoder::speedRPM() const {
    return rpm; // Return the last stored RPM value
}
