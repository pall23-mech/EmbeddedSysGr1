#include <context.h>
#include <initialization_state.h>
#include <operational_state.h>
#include <stopped_state.h>
#include <Analog_out.h>
// Create an encoder instance
// operational_state.cpp
extern Encoder encoder;           // Declare the encoder instance
extern float targetPPS;           // Declare targetPPS
extern unsigned long lastControlUpdate; // Declare lastControlUpdatet
extern Analog_out motorPWM;    // Use pin 9 for PWM control
// Create an Analog_out instance directly for the direction control pin
//Analog_out directionControl(8);

void OperationalState::on_entry()
    {
            // Initialize PWM on Pin D9
        motorPWM.init(1);  // Initialize with a dummy period for now
        Serial.println("PWM initialized on Pin D9.");


        Serial.println("Entering Operational state, available commands:");
        Serial.println("'r' - Reset (transition to Initilaization)");
        digitalWrite(LED_BUILTIN, HIGH); // Turn LED on in operational state        context_->getEncoder().init();     // Initialize the encoder

    }

    void OperationalState::on_exit()
    {
        Serial.println("Exiting Operational state");
        digitalWrite(LED_BUILTIN, LOW);
    }

    void OperationalState::on_do()
    {
        Serial.print(targetPPS);
    unsigned long currentTime = millis();
   // if (currentTime - context_->getLastControlUpdate() >= context_->getControlInterval()) {
        Serial.println("Executing control loop...");
     //   context_->getLastControlUpdate() = currentTime;
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