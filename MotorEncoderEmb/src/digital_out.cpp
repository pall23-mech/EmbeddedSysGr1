#include "digital_out.h"
#include <avr/io.h>

DigitalOut::DigitalOut(uint8_t mask)
 {
   pinMask=(1<< mask); // Constructor body
}

void DigitalOut::init() {
    DDRB |= pinMask;  // Set the pin corresponding to pinMask as output
}

void DigitalOut::toggle() {
    PORTB ^= pinMask;  // Toggle the pin state using XOR
}

void DigitalOut::set_hi() {
    PORTB |= pinMask;  // Set the pin high
}

void DigitalOut::set_lo() {
    PORTB &= ~pinMask;  // Set the pin low
}
