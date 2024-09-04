#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>

class Encoder
{
public: 
    Encoder(int pin_A, int pin_B);  // Constructor to initialize the encoder with two pins

    void init();                    // Initializes the encoder pins and attaches interrupts
    int position() const;           // Returns the current encoder position count

private:
    int encoder_pin_A;              // Pin for the first encoder pin (C1)
    int encoder_pin_B;              // Pin for the second encoder pin (C2)
    volatile int count;             // Volatile counter for encoder position
};

#endif // ENCODER_H

