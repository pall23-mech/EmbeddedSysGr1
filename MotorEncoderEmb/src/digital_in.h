#ifndef DIGITAL_IN_H
#define DIGITAL_IN_H

#include <stdint.h>

class DigitalIn {  // Make sure the class name is DigitalOut
public:
    // Constructor to initialize the pin mask
    DigitalIn(uint8_t mask);

    // Initializes the digital input pin as input
    void init();

    bool is_hi();

    bool is_lo(); 

private:
    uint8_t pinMask;  // Private member variable to store the pin mask
};

#endif // DIGITAL_IN_H
