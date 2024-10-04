// pi_controller.cpp
#include "pi_controller.h"

// Constructor implementation
PI_Controller::PI_Controller(float Kp_value, float Ti_value, float T_step)
    : P_controller(Kp_value), Ti(Ti_value), T(T_step), integralError(0) {}

// Method to check if the control signal is saturated
bool PI_Controller::isSaturated(double controlSignal) {
    return controlSignal >= 100.0 || controlSignal <= 0.0;
}

// Override the update function to implement the PI control law
double PI_Controller::update(double ref, double actual) {
    double error = ref - actual;

    // Compute the control signal using the PI control law
    double controlSignal = Kp * (error + (1 / Ti) * integralError);

    // Stop integrating if the control signal is saturated
    if (!isSaturated(controlSignal)) {
        integralError += error * T;  // Accumulate the integral of the error
    }

    // Clamp the control signal between 0% and 100%
    if (controlSignal > 100.0) {
        controlSignal = 100.0;
    } else if (controlSignal < 0.0) {
        controlSignal = 0.0;
    }

    return controlSignal;
}
