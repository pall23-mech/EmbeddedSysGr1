#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>
#include "digital_in.h"

class Encoder
{
public: 
    Encoder(int pin_A, int pin_B);  // Constructor to initialize the encoder with two pins

    void init();                    // Initializes the encoder pins as inputs
    void update();                  // Updates the encoder position count based on pin states
    int position() const;           // Returns the current encoder position count

private:
    DigitalIn encoder_pin_A;        // Digital input for the first encoder pin (C1)
    DigitalIn encoder_pin_B;        // Digital input for the second encoder pin (C2)
    int count;                      // Counter for encoder position
    bool last_state_A;              // Last state of the first encoder pin (C1)
    bool last_state_B;              // Last state of the second encoder pin (C2)
};

#endif // ENCODER_H
