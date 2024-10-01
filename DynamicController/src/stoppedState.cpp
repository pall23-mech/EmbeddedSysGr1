#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>


void StoppedState::on_entry()
    {
        Serial.println("Entering Stopped state, available commands:");
        Serial.println("'t' - Transition (transition to Operational)");
        Serial.println("'r' - Reset (transition to Initilaization)");
    }

    void StoppedState::on_exit()
    {
        Serial.println("Exiting Stopped state");
        digitalWrite(LED_BUILTIN, LOW);
    }

    void StoppedState::on_do()
    {
        // operational behavior
        // Blink at 2 HZ
    }

    void StoppedState::on_reset()
    {
        Serial.println("Reset command, transitioning to Initializaiton state");
        this->context_->transition_to(new InitializationState()); //System reset
    }

    void StoppedState::on_transition()
    {
        //Serial.println("Transition to pre-operational state");
        //this->context_->transition_to(new preOperationalState());
        // Will transition to the preOperationalState later
    }

    void StoppedState::on_fault()
    {
        Serial.println("Fault detection, transitioning to Stopped state");
        this->context_->transition_to(new StoppedState());
    }