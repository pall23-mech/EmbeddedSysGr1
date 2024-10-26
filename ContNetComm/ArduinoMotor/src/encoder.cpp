#include "encoder.h"
#include <Arduino.h>

// Global volatile variable to keep track of the encoder count
volatile int encoderPosition = 0; // Position of the encoder

// Static variables for the encoder pins (interrupts need them to be global)
static int encoderPinA;
static int encoderPinB;
bool last_stateA = false;
bool last_stateB = false;
static int pulses_per_rev;


// Interrupt Service Routine
ISR(PCINT1_vect)
{
    // Read the current state of both encoder pins
    bool state_A = digitalRead(encoderPinA);
    bool state_B = digitalRead(encoderPinB);

    // Determine direction based on the states of A and B
    if (state_A != last_stateA)
    {
        if (state_B != state_A)
        {
            encoderPosition++; // Clockwise rotation
        }
        else
        {
            encoderPosition--; // Counterclockwise rotation
        }
    }
    PORTB ^= (1 << PB5); // LED toggle
    last_stateA = state_A;
    last_stateB = state_B;
}

Encoder::Encoder(int pin_A, int pin_B, int ppr)
    : encoder_pin_A(pin_A), encoder_pin_B(pin_B), pulses_per_rev(ppr), count(0), last_position(0), last_time(0)
{
    // Constructor body
}

void Encoder::init()
{
    // Store the pins in static variables for ISR access
    encoderPinA = encoder_pin_A;
    encoderPinB = encoder_pin_B;

    // Initialize pins as inputs
    pinMode(encoder_pin_A, INPUT);
    pinMode(encoder_pin_B, INPUT);

    // Enable Pin Change Interrupts for A2 (PCINT10) and A3 (PCINT11)
    PCICR |= (1 << PCIE1);    // Enable Pin Change Interrupts for PCINT[14:8] (which includes A2 and A3)
    PCMSK1 |= (1 << PCINT10); // Enable interrupt for A2 (PCINT10)
    PCMSK1 |= (1 << PCINT11); // Enable interrupt for A3 (PCINT11)

    last_time = millis(); // Initialize the last time
}

int Encoder::position() const
{
    return encoderPosition; // Return the current position count
}

void Encoder::updateSpeed()
{
    unsigned long currentTime = millis();
    unsigned long deltaTime = currentTime - last_time;

    if (deltaTime > 0)
    {
        int deltaPosition = encoderPosition - last_position;
        pps = (float)deltaPosition / (deltaTime / 1000.0); // Calculate pulses per second
        rpm = (pps * 60.0) / pulses_per_rev;

        // Update last position and time
        last_position = encoderPosition;
        last_time = currentTime;
    }
}

float Encoder::speedPPS() const
{
    return pps; // Return the last stored PPS value
}

float Encoder::speedRPM() const
{
    return rpm; // Return the last stored RPM value
}