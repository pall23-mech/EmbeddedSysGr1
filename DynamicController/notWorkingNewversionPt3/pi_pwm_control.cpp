// pwm_control.cpp
#include "pwm_control.h" // Include the corresponding header file
#include "analog_out.h"  // Include the analog out header
#include "p_controller.h" // Include the P_controller header
#include <context.h> // Include the P_controller header
#include "pi_controller.h"
#define CONTROL_PERIOD 3 // Control update every 3ms
// Create a PI_Controller instance (with Kp, Ti, and T_step)
PI_Controller piController(2.1, 0.5, 0.01);  // Example values


// Declare external variables to be used in this file
extern Encoder encoder; // Encoder object
extern float Kp;
extern float Ti;
extern unsigned long lastControlUpdate; // Time of the last control update

// Create an Analog_out instance for the PWM pin
Analog_out motorPWM(9); // Use pin 9 for PWM control

// Create a P_controller instance (move Kp from main to here)
P_controller pController(2.5); // Use the desired Kp value

void PwmControl::setupPWM_Timer1() {
    // Initialize the analog output (PWM) with a period
    motorPWM.init(2); // Assuming a period of 2 ms (500 Hz) for the PWM signal
}
void PwmControl::stopMotor() {
    // Set the duty cycle to 0 to stop the motor
    motorPWM.set(0.0);
    Serial.println("Motor stopped, PWM duty cycle set to 0.");
}
void PwmControl::controlLoop() {  // change to controlLoop() again later....
    unsigned long currentTime = millis();
    float targetPPS = context_->getTargetPPS();
    float Kp = context_->getKp();
    float Ti = context_->getTi();

    if (currentTime - lastControlUpdate >= CONTROL_PERIOD) {  // Control update every 3 ms
        encoder.updateSpeed();  // Update the speed (PPS and RPM) based on the current position and time

        float actualPPS = encoder.speedPPS();
        
        // Use the PI_Controller to calculate the control signal
        double controlSignal = piController.update(targetPPS, actualPPS);

        if (controlSignal > 100){
            controlSignal = 100;
        }
        // Ensure the control signal is scaled appropriately for the motorPWM (assuming it expects 0.0 to 1.0 range)
        float dutyCycle = constrain(controlSignal / targetPPS, 0.0, 1.0);  // Normalize to PWM range

        // Set the PWM duty cycle using Analog_out
        motorPWM.set(dutyCycle);

        // Debugging output
        Serial.print("Target PPS: ");
        Serial.println(targetPPS);

        Serial.print("Actual PPS: ");
        Serial.println(actualPPS);

        Serial.print("Control Signal (PWM Duty): ");
        Serial.println(dutyCycle * 100);  // Display as percentage

        // Update last control time
        lastControlUpdate = currentTime;
    }
}
