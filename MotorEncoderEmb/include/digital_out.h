#ifndef DIGITAL_OUT_H
#define DIGITAL_OUT_H

#include <stdint.h>

class DigitalOut {  // Make sure the class name is DigitalOut
public:
    // Constructor to initialize the pin mask
    DigitalOut(uint8_t mask);

    // Initializes the digital output pin as output
    void init();

    // Sets the digital output pin high
    void set_hi();

    // Sets the digital output pin low
    void set_lo();

    // Toggles the state of the digital output pin
    void toggle();

private:
    uint8_t pinMask;  // Private member variable to store the pin mask
};

#endif // DIGITAL_OUT_H
