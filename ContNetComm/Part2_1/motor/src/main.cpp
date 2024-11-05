#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include <Arduino.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"
// #include "P_controller.h"

Timer_msec mainTimer;
Digital_out motorDirectionPin(0); // pin D8
Analog_out motorSpeedPin(1);  // PWM pin D9
Encoder motorEncoder(3, 4);   // encoder pin D11 D12
float proportionalGain = 0.01;
float integralTime = 5;
PI_controller speedController(proportionalGain, integralTime);
// P_controller speedController(proportionalGain);

unsigned long lastUpdateTimestamp = 0;
int inputAnalogPin = A3; 
double referenceSpeed = 0;
double currentSpeed = 0;
double pwmOutput = 0;
double controlSignal = 0;
bool isSpeedHigh = false; // Track state of referenceSpeed

int main() {  
    init(); // Initialize Arduino framework
    Serial.begin(115200, SERIAL_8N1);  
    mainTimer.init(0.1); // ms
    sei();  // enable interrupts

    motorEncoder.init();
    motorDirectionPin.init();
    motorSpeedPin.init(10);  // ms
    motorSpeedPin.set(0);  // duty cycle
    motorDirectionPin.set_lo();
    
    while (1) {
        String receivedCommand;
        char serialBuffer[100]; 
        receivedCommand = Serial.readStringUntil('\0'); // C strings end with \0
        if (receivedCommand.substring(0, 2) == "MO") {   // begins with "MO"?
            if (!isSpeedHigh) {
                referenceSpeed = 100;
                isSpeedHigh = true;
            } else {
                referenceSpeed = 0;
                isSpeedHigh = false;
            }
            sprintf(serialBuffer, "MO :Reference set to %d\n", (int)referenceSpeed);
        } else {
            sprintf(serialBuffer, "MO :Unknown command: %s", receivedCommand.c_str());
        }
        Serial.print(serialBuffer); // send the buffer to the RPi
        currentSpeed = abs(motorEncoder.speed());
        controlSignal = speedController.update(referenceSpeed, currentSpeed);
        controlSignal = constrain(controlSignal, 0.0, 0.999); // Ensure controlSignal is within [0, 1]
        pwmOutput = controlSignal;
        motorSpeedPin.set(pwmOutput);
        motorEncoder.update();
        _delay_ms(3);
    }
    return 0;
}

ISR (INT0_vect) {
    motorEncoder.update();
}

ISR(TIMER1_COMPA_vect) {
    motorSpeedPin.pin.set_hi();      
}

ISR(TIMER1_COMPB_vect) {
    motorSpeedPin.pin.set_lo(); 
}
