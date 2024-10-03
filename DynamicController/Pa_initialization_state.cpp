#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>


void InitializationState::on_entry()
    {
        digitalWrite(LED_BUILTIN, LOW); //initialize system, just LED for now
        Serial.println("Entering Initialization state; Initializing parameters...");
        Serial.println("Available commrands:"); // will be taken out (no commmands in this state?)
        Serial.println("'t' - Transition (transition to Operational)"); // Will be autononmous later

        // initailize here

        context_->getEncoder().init();     // Initialize the encoder

        context_->getDirectionControl().init(1); // Initalize Analog_out (direction control pin)

        context_->getDirectionControl().set(0); // Set initial direction (forward), LOW for forward

        setupPWM_Timer1(); // Set up Timer1 for motor speed control (PWM)

        // Initialize the control parameters
        context_->getTargetPPS() = 2200.0; // Set esired speed
        context_->getKp() = 2.1; // Set proportional gain for the controller
        context_->getLastControlUpdate() = millis(); // Initalize the timing variable

        // we will later implement below: (take out transition..)
        //Serial.println("Initalization complete, transitioning to Pre-Operational State.");
        //this->context_->transition_to(new OperationalState());
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

    void InitializationState::on_fault()
    {
        Serial.println("Fault detection, transitioning to Stopped state");
        this->context_->transition_to(new StoppedState());
    }