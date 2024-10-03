// pwm_control.cpp
#include "pwm_control.h" // Include the corresponding header file
#include "analog_out.h"  // Include the analog out header
#include "p_controller.h" // Include the P_controller header

// Declare external variables to be used in this file
extern Encoder encoder; // Encoder object
extern float targetPPS; // Desired speed
extern float Kp;
extern unsigned long lastControlUpdate; // Time of the last control update

#define CONTROL_PERIOD 3 // Control update every 3ms

// Create an Analog_out instance for the PWM pin
Analog_out motorPWM(9); // Use pin 9 for PWM control

// Create a P_controller instance (move Kp from main to here)
P_controller pController(2.5); // Use the desired Kp value


void setupPWM_Timer1() {
    // Initialize the analog output (PWM) with a period
    motorPWM.init(2); // Assuming a period of 2 ms (500 Hz) for the PWM signal
}

void controlLoop() {
    unsigned long currentTime = millis();

    if (currentTime - lastControlUpdate >= CONTROL_PERIOD) { // Control update every 3 ms
        encoder.updateSpeed(); // Update the speed (PPS and RPM) based on the current position and time

        float actualPPS = encoder.speedPPS();
        
        // Use the P_controller to calculate the control signal
        double controlSignal = pController.update(targetPPS, actualPPS);

        // Calculate the duty cycle and constrain it within the allowed range (0.0 to 1.0 for Analog_out)
        float dutyCycle = constrain(controlSignal / targetPPS, 0.0, 1.0);

        // Set the PWM duty cycle using Analog_out
        motorPWM.set(dutyCycle);

        // Debugging output
        Serial.print("Target PPS: ");
        Serial.println(targetPPS);

        Serial.print("Actual PPS: ");
        Serial.println(actualPPS);

        Serial.print("Control Signal (PWM Duty): ");
        Serial.println(dutyCycle * 100); // Display as percentage

        lastControlUpdate = currentTime;
    }
}
