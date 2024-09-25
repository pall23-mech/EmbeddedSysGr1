#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>


void OperationalState::on_entry()
    {
        Serial.println("Entering Initialization state, available commands:");
        Serial.println("'c' - Too Cool (transition to Heating)");
        Serial.println("'h' - Too Hot (transition to Cooling)");
    }

    void OperationalState::on_exit()
    {
        Serial.println("Exiting Initialization state");
    }

    void OperationalState::on_do()
    {
        // operational behavior
    }

    void OperationalState::on_event1()
    {
        Serial.println("Event: Too cool. Transitioning to Heating...");
        this->context_->transition_to(new InitializationState());
    }

    void OperationalState::on_event2()
    {

    }