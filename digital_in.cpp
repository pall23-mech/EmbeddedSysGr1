#include "digital_in.h"
#include <avr/io.h>

DigitalIn::DigitalIn(uint8_t mask) {
    pinMask = (1 << mask); // Constructor body
}

void DigitalIn::init() {
    DDRB &= ~pinMask;  // Set the pin as input
    PORTB |= pinMask;  // Enable the internal pull-up resistor
}

bool DigitalIn::is_hi() {
    return (PINB & pinMask) != 0;  // Check if the pin is high
}

bool DigitalIn::is_lo() {
    return (PINB & pinMask) == 0;  // Check if the pin is low
}