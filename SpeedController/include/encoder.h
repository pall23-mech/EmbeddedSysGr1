#ifndef ENCODER_H
#define ENCODER_H

#include <avr/io.h>

class Encoder
{
public: 
    Encoder(int pin_A, int pin_B, int pulses_per_revolution);  // Constructor to initialize the encoder with two pins and pulses per revolution

    void init();                    // Initializes the encoder pins and attaches interrupts
    int position() const;           // Returns the current encoder position count
    float speedPPS();               // Returns the speed in pulses per second (PPS)
    float speedRPM();               // Returns the speed in revolutions per minute (RPM)

private:
    int encoder_pin_A;              // Pin for the first encoder pin (C1)
    int encoder_pin_B;              // Pin for the second encoder pin (C2)
    float pulses_per_rev;             // Number of pulses per revolution (PPR)
    volatile int last_position;     // Last encoder position to calculate speed
    volatile unsigned long last_time; // Time of the last postion update
    volatile int count;             // Volatile counter for encoder position
};

#endif // ENCODER_H

