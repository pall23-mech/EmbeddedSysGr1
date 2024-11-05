#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include "p_controller.h"

class PI_Controller : public P_controller {
public:
    PI_Controller(float Kp_value, float Ti_value, float T_step);

    // Override the update method to implement PI control
    double update(double ref, double actual) override;

private:
    float Ti;     // Integration time constant
    float T;      // Time step for integration
    double integralError;  // Accumulated error (integral of error)

    // Declare isSaturated method
    bool isSaturated(double controlSignal);  // <--- Add this line to the class declaration
};

#endif // PI_CONTROLLER_H
