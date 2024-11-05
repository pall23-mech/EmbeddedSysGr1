#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>
#include <pwm_control.h>
#include <pre_operational_state.h>


void InitializationState::on_entry()
    {
        digitalWrite(LED_BUILTIN, LOW); //initialize system, just LED for now
        Serial.println("Entering Initialization state; Initializing parameters...");

        // initailize here

        // should we initalize the pwm controller here?

        context_->getEncoder().init();     // Initialize the encoder

        context_->getDirectionControl().init(1); // Initalize Analog_out (direction control pin)

        context_->getDirectionControl().set(0); // Set initial direction (forward), LOW for forward

        // Initialize the control parameters
        context_->setTargetPPS(2200.0); // Set desired speed
        context_->getTargetPPS();

        context_->getLastControlUpdate() = millis(); // Initalize the timing variable

        Serial.println("Initalization complete, transitioning to Pre-Operational State.");
        this->context_->transition_to(new PreOperationalState());

    }

    void InitializationState::on_exit()
    {
        Serial.println("Exiting Initialization state");
    }

    void InitializationState::on_do()
    {

    }

    void InitializationState::on_reset()
    {

    }

    void InitializationState::on_pre_transition()
    {

    }

    void InitializationState::on_op_transition()
    {
        
    }

    void InitializationState::on_fault()
    {

    }
    
    void InitializationState::on_configure()
    {

    }