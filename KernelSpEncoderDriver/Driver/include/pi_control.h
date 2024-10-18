#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

// PI Controller structure
typedef struct {
    float kp;          // Proportional gain
    float ki;          // Integral gain
    float integral;    // Integral term accumulator
    float setpoint;    // Desired setpoint value
} PIController;

// Function prototypes
void pi_controller_init(PIController *controller, float kp, float ki, float setpoint);
float pi_controller_update(PIController *controller, float measurement, float dt);

#endif // PI_CONTROLLER_H
