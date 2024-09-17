// pwm_control.cpp
#include "pwm_control.h" // Include the corresponding header file

#define PWM_PIN 9        // Pin for PWM control of the motor
#define PWM_FREQ 500     // 500Hz PWM frequency
#define CONTROL_PERIOD 10 // Control update every 10ms

// Declare external variables to be used in this file
extern Encoder encoder; // Encoder object
extern float targetPPS; // Desired speed
extern float Kp; // Proportional gain
extern unsigned long lastControlUpdate; // Time of the last control update

void setupPWM_Timer1() {
    pinMode(PWM_PIN, OUTPUT); // Configure Timer 1 for 500 Hz on pin 9

    // Set Timer1 to Fast PWM mode with a top value for the desired frequency
    TCCR1A = (1 << WGM11) | (1 << COM1A1); // Fast PWM, non-inverted
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Fast PWM, Prescaler = 64
    ICR1 = (F_CPU / (64 * PWM_FREQ)) - 1; // Set TOP value for the desired PWM frequency
}

void controlLoop() {
    unsigned long currentTime = millis();

    if (currentTime - lastControlUpdate >= CONTROL_PERIOD) { // Control update every 10 ms
        encoder.updateSpeed(); // Update the speed (PPS and RPM) based on the current position and time

        float actualPPS = encoder.speedPPS();
        float error = targetPPS - actualPPS;

        float controlSignal = Kp * error; // Proportional control law

        // Calculate the duty cycle and constrain it within the allowed range
        int dutyCycle = constrain(map(controlSignal, 0, targetPPS, 0, 499), 0, 499);

        OCR1A = dutyCycle; // Apply PWM to pin 9 (Timer1, OCR1A controls duty cycle)

        // Debugging output
        Serial.print("Target PPS: ");
        Serial.println(targetPPS);

        Serial.print("Actual PPS: ");
        Serial.println(actualPPS);

        Serial.print("Control Signal (PWM Duty): ");
        Serial.println(dutyCycle);

        lastControlUpdate = currentTime;
    }
}
