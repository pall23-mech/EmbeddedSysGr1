// analog_out.h
#ifndef ANALOG_OUT_H
#define ANALOG_OUT_H

class Analog_out {
public:
    Analog_out(int pin_no); // Constructor
    void init(int period_ms); // Initialize PWM with a specific period in milliseconds
    void set(float duty_cycle); // Set the duty cycle (0.0 to 1.0 range)

private:
    int pin;
    int timerTopValue; // Store the timer top value for PWM frequency
};

#endif
