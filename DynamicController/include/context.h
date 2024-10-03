#ifndef CONTEXT_H
#define CONTEXT_H

#include <encoder.h>
#include <Analog_out.h>

class State;

class Context
{
private:
    State *state_;

    // Shared variables
    Encoder encoder; // Encoder object
    Analog_out directionControl; // Direction control pin for motor
    float targetPPS; // Target speed in pulses per second
    float Kp; // Proportional gain
    unsigned long lastControlUpdate; // Last control loop update time
    unsigned long controlInterval; // Control loop interval

public:
    Context(State *state);
    ~Context();

    void transition_to(State *state);
    void do_work();

    void reset();
    void transition(); // change later...
    void fault();

    // For shared variables, getters and setters
    Encoder& getEncoder() {return encoder;}
    Analog_out& getDirectionControl() {return directionControl;}
    float& getTargetPPS() {return targetPPS;}
    float& getKp() {return Kp;}
    void setTargetPPS(float target) {targetPPS = target;}
    unsigned long& getLastControlUpdate() {return lastControlUpdate;}
    unsigned long& getControlInterval() {return controlInterval;}
};

#endif // CONTEXT_H