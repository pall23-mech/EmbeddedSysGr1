#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>


void InitializationState::on_entry()
    {
        digitalWrite(LED_BUILTIN, LOW); //initialize system, just LED for now
        Serial.println("Entering Initialization state, available commands:");
        Serial.println("'t' - Transition (transition to Operational)");
    }

    void InitializationState::on_exit()
    {
        Serial.println("Exiting Initialization state");
    }

    void InitializationState::on_do()
    {
        // initialization behavior
    }

    void InitializationState::on_reset()
    {

    }

    void InitializationState::on_transition()
    {
        Serial.println("Event: Transition. Transitioning to Operational...");
        this->context_->transition_to(new OperationalState());
    }