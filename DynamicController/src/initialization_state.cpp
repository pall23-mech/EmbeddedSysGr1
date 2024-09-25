#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>


void InitializationState::on_entry()
    {
        Serial.println("Entering Initialization state, available commands:");
        Serial.println("'c' - Too Cool (transition to Heating)");
        Serial.println("'h' - Too Hot (transition to Cooling)");
    }

    void InitializationState::on_exit()
    {
        Serial.println("Exiting Initialization state");
    }

    void InitializationState::on_do()
    {
        // initialization behavior
    }

    void InitializationState::on_event1()
    {
        Serial.println("Event: Too cool. Transitioning to Heating...");
        this->context_->transition_to(new OperationalState());
    }

    void InitializationState::on_event2()
    {

    }