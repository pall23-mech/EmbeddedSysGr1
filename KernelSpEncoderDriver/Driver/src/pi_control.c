#include <stdio.h>

// PI Controller structure
typedef struct {
    float kp;         // Proportional gain
    float ki;         // Integral gain
    float integral;   // Accumulated integral value
    float setpoint;   // Desired setpoint
} PIController;

// Initialize the PI Controller
void pi_controller_init(PIController *controller, float kp, float ki, float setpoint) {
    controller->kp = kp;
    controller->ki = ki;
    controller->integral = 0;
    controller->setpoint = setpoint;
}

// Update the PI Controller based on the measured value and time difference
float pi_controller_update(PIController *controller, float measurement, float dt) {
    float error = controller->setpoint - measurement;
    controller->integral += error * dt; // Accumulate the integral
    return (controller->kp * error) + (controller->ki * controller->integral);
}
