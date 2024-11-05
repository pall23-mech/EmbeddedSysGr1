#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>
#include <pre_operational_state.h>
#include "analog_out.h"  // Include the analog out header

extern Encoder encoder;
extern float targetPPS;
extern unsigned long lastControlUpdate;
extern Analog_out motorPWM;


void OperationalState::on_entry()
    {
        Serial.println("Entering Operational state, available commands:");
        Serial.println("'r' - Reset (transition to Initilaization)");
        Serial.println("'p' - Transition (transition to PreOperational)");
        digitalWrite(LED_BUILTIN, HIGH); // Turn LED on in operational state

        motorPWM.init(1);
    }

    void OperationalState::on_exit()
    {
        Serial.println("Exiting Operational state");
        digitalWrite(LED_BUILTIN, LOW);  // Turn off LED

        // Stop the motor by setting PWM duty cycle to 0
        context_->getPwmControl()->stopMotor();  // Call the new stop function to stop the motor

        // Optionally re-initialize the encoder or other components
        encoder.init();
    }

    void OperationalState::on_do()
    {   
        
        context_->getPwmControl()->controlLoop();
            // Define the message to be sent through UART
        uint8_t msg[8] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x39, 0x84};

        // Send the message through UART each control loop iteration
        Serial.write(msg, sizeof(msg));
    }

    void OperationalState::on_reset()
    {
        Serial.println("Reset command, transitioning to Initializaiton state");
        this->context_->transition_to(new InitializationState()); //System reset
    }

    void OperationalState::on_pre_transition()
    {
        Serial.println("Transition to pre-operational state");
        this->context_->transition_to(new PreOperationalState());
    }

    void OperationalState::on_op_transition()
    {

    }

    void OperationalState::on_fault()
    {
        Serial.println("Fault detection, transitioning to Stopped state");
        this->context_->transition_to(new StoppedState());
    }

    void OperationalState::on_configure()
    {
        
    }