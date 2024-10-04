// pwm_control.h
#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#include <Arduino.h> // Include Arduino header for types and macros
#include "encoder.h" // Include the encoder header for encoder functions

class Context;

class PwmControl {
private: 
    Context* context_; // Pointer to Context

public:

    PwmControl(Context* ctx) : context_(ctx) {}

    // Function to initialize Timer1 for PWM
    void setupPWM_Timer1();
    void stopMotor();
    
    // Control loop for PWM adjustment
    void controlLoop();
    
};

#endif // PWM_CONTROL_H
