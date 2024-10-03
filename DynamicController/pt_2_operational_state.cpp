#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>
#include "analog_out.h"  // Include the analog out header

extern Encoder encoder;
extern float targetPPS;
extern unsigned long lastControlUpdate;
extern Analog_out motorPWM;


void OperationalState::on_entry()
    {
        Serial.println("Entering Operational state, available commands:");
        Serial.println("'r' - Reset (transition to Initilaization)");
        digitalWrite(LED_BUILTIN, HIGH); // Turn LED on in operational state

        motorPWM.init(1);
    }

    void OperationalState::on_exit()
    {
        Serial.println("Exiting Operational state");
        digitalWrite(LED_BUILTIN, LOW);  // Turn off LED

        // Stop the motor by setting PWM duty cycle to 0
        stopMotor();  // Call the new stop function to stop the motor

        // Optionally re-initialize the encoder or other components
       // encoder.init();
    }

    void OperationalState::on_do()
    {   
        controlLoop();
    }

    void OperationalState::on_reset()
    {
        Serial.println("Reset command, transitioning to Initializaiton state");
        this->context_->transition_to(new InitializationState()); //System reset
    }

    void OperationalState::on_transition()
    {
        //Serial.println("Transition to pre-operational state");
        //this->context_->transition_to(new preOperationalState());
        // Will transition to the preOperationalState later
    }

    void OperationalState::on_fault()
    {
        Serial.println("Fault detection, transitioning to Stopped state");
        this->context_->transition_to(new StoppedState());
    }