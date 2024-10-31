// analog_out.cpp
#include <Arduino.h>
#include "analog_out.h"

// Constructor that initializes the pin
Analog_out::Analog_out(int pin_no) : pin(pin_no) {}

// Initialize the timer and pin
void Analog_out::init(int period_ms) {
    pinMode(pin, OUTPUT); // Set pin as output

    // Calculate the top value for the desired period in milliseconds
    timerTopValue = (F_CPU / (64 * 1000 / period_ms)) - 1; // Assuming prescaler of 64

    // Configure Timer1 for Fast PWM mode on the specified pin (pin 9 or 10 on Arduino Uno)
    TCCR1A = (1 << WGM11) | (1 << COM1A1); // Fast PWM, non-inverted
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Fast PWM, Prescaler = 64
    ICR1 = timerTopValue; // Set the TOP value for the desired PWM frequency
}

// Set the duty cycle (0-1.0 range)
void Analog_out::set(float duty_cycle) {
    // Calculate the compare match value based on duty cycle
    int compareMatchValue = static_cast<int>(duty_cycle * timerTopValue);

    // Apply PWM to pin 9 or 10 (Arduino Uno uses OCR1A or OCR1B for Timer1)
    if (pin == 9) {
        OCR1A = compareMatchValue; // Set duty cycle on pin 9
    } else if (pin == 10) {
        OCR1B = compareMatchValue; // Set duty cycle on pin 10
    }
}
