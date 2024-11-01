// p_controller.h
#ifndef P_CONTROLLER_H
#define P_CONTROLLER_H

class P_controller {
public:
    // Constructor that takes Kp as an argument
    P_controller(float Kp_value);
    
    // Update function to compute control signal
    virtual double update(double ref, double actual);

protected:
    float Kp; // Proportional gain
};

#endif
