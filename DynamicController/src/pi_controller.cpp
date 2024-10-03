// pi_controller.cpp
#include "pi_controller.h"

// Constructor initializes Kp, Ti, T, and integralError
PI_Controller::PI_Controller(float Kp_value, float Ti_value, float T_step)
    : P_controller(Kp_value), Ti(Ti_value), T(T_step), integralError(0) {}

// Update function implements the PI control law
double PI_Controller::update(double ref, double actual) {
   // Calculate the error
    double error = ref - actual;

    // If the control signal is saturated, do not accumulate the integral
    if (controlSignal < 100.0 && controlSignal > 0.0) {
        integralError += error * T;  // Accumulate the integral of the error
    }

    // Compute the control signal (PI control law)
    double controlSignal = Kp * (error + (1 / Ti) * integralError);

    // Clamp controlSignal to 0-100% PWM range
    if (controlSignal > 100.0) {
        controlSignal = 100.0;
    } else if (controlSignal < 0.0) {
        controlSignal = 0.0;
    }

    return controlSignal;

    
}
