#ifndef INITIALIZATION_STATE_H
#define INITIALIZATION_STATE_H

#include <state.h>

#include <encoder.h>
#include <pwm_control.h> // Include the PWM control header
#include <analog_out.h> // Include the Analog_out header

class InitializationState : public State
{
public:
    void on_entry() override;
    void on_exit() override;
    void on_do() override;

    // change on_eventX to our event names
    void on_reset() override; 
    void on_transition() override;
};

#endif // INITIALIZATION_STATE_H