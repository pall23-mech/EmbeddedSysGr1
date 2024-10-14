#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>
#include <pre_operational_state.h>


void StoppedState::on_entry()
    {
        Serial.println("Entering Stopped state, available commands:");
        Serial.println("'o' - Transition (transition to Operational)");
        Serial.println("'p' - Transition (transition to PreOperational)");
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
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500 / 2);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500 / 2);
    }

    void StoppedState::on_reset()
    {
        Serial.println("Reset command, transitioning to Initializaiton state");
        this->context_->transition_to(new InitializationState()); //System reset
    }

    void StoppedState::on_pre_transition()
    {
        Serial.println("Transition to pre-operational state");
        this->context_->transition_to(new PreOperationalState());
    }

    void StoppedState::on_op_transition()
    {
        Serial.println("Transition to Operational state");
        this->context_->transition_to(new OperationalState());
    }

    void StoppedState::on_fault()
    {   
        // If already in fault state, no need to retrigger
    }

    void StoppedState::on_configure()
    {
        
    }