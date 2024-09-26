#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>


void OperationalState::on_entry()
    {
        Serial.println("Entering operational state");
        digitalWrite(LED_BUILTIN, HIGH); // Turn LED on in operational state
    }

    void OperationalState::on_exit()
    {
        Serial.println("Exiting operational state state");
        digitalWrite(LED_BUILTIN, LOW);
    }

    void OperationalState::on_do()
    {
        // operational behavior
        Serial.println("Motor operating normally");
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