// p_controller.cpp
#include "p_controller.h"

// Constructor implementation
P_controller::P_controller(float Kp_value) : Kp(Kp_value) {}

// Update function implementation
double P_controller::update(double ref, double actual) {
    double error = ref - actual; // Calculate the error
    double controlSignal = Kp * error; // Proportional control law
    return controlSignal;
}
