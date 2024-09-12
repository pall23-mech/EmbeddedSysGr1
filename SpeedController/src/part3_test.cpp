#include <Arduino.h>
#include "encoder.h"

#define PWM_PIN 9 // Pin for PWM control of the motor
#define AIN2_PIN 8 // Pin for the motor direction control (Ain2)
#define CONTROL_PERIOD 10 // Control update every 10ms
#define PWM_FREQ 500 // 500Hz PWM frequency

// Initialize the encoder on A2 (PCINT10, pin 16) and A3 (PCINT11, pin 17) and 1400 pulses per rev, from the specifications
Encoder encoder(16, 17, 1400.0);

float targetPPS = 1000.0; // Desired speed (commanded reference speed)  (we can also use RPM)
float Kp = 0.5; // Proportional gain for the controller
unsigned long lastControlUpdate = 0; // Time of the last control update

void setupPWM_Timer1()
{
    pinMode(PWM_PIN, OUTPUT); // Configure Timer 1 for 500 Hz on pin 9

    // Set Timer1 to Fast PWM mode with a top value of 499 for 500 Hz
    TCCR1A = (1 << WGM11) | (1 << COM1A1); // Fast PWM, non-inverted
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10); // Fast PWM, Prescaler = 64
    ICR1 = 499; // Set TOP value for 500 Hz
}

void setup()
{
    Serial.begin(9600); // Start serial communication at 9600 baud
    encoder.init();     // Initialize the encoder and interrupts

    setupPWM_Timer1(); // Set up timer1 for PWM on pin 9

    pinMode(AIN2_PIN, OUTPUT); // Set Ain2 as an output pin

    digitalWrite(AIN2_PIN, LOW); // LOW for one direction, change to HIGH for reverse direction
}

void controlLoop()
{
    unsigned long currentTime = millis();

    if (currentTime - lastControlUpdate >= CONTROL_PERIOD) // control update every 10 ms
    {
        encoder.updateSpeed(); // Update the speed (PPS and RPM) based on the current position and time

        float actualPPS = encoder.speedPPS();
        float error = targetPPS - actualPPS;

        float controlSignal = Kp * error; // Proportional control law

        int dutyCycle = constrain(map(controlSignal, 0, targetPPS, 0, 499), 0, 499);

        OCR1A = dutyCycle; // Apply PWM to pin 9 (Timer1, OCR1A controls duty cycle)


        Serial.print("Target PPS: ");
        Serial.println(targetPPS);

        Serial.print("Actual PPS: ");
        Serial.println(actualPPS);

        Serial.print("Control Signal (PWM Duty): ");
        Serial.println(dutyCycle);

        lastControlUpdate = currentTime;
    }
}

int main()
{
    
    //take position comparison,
    // Initialize Arduino core functions
    init(); // IMPORTANT: Initialize Arduino core libraries (including Serial)

    // Call the setup() function
    setup();

    // Infinite loop that continuously calls loop() as Arduino normally would
    while (1)
    {
        controlLoop();
    }

    return 0;
}
