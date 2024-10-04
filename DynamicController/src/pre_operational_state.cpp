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


void PreOperationalState::on_entry()
    {
        Serial.println("Entering Pre Operational state, available commands:");
        Serial.println("'c' - Configuration");
        Serial.println("'r' - Reset (transition to Initilaization)");
        Serial.println("'o' - Transition (transition to Operational)");
        digitalWrite(LED_BUILTIN, HIGH); // Turn LED on in operational state

        motorPWM.init(1);
    }

    void PreOperationalState::on_exit()
    {
        Serial.println("Exiting Pre Operational state");
        digitalWrite(LED_BUILTIN, LOW);  // Turn off LED

    }

    void PreOperationalState::on_do()
    {   
        // Blink at 1 HZ
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500 / 1);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500 / 1);
    }

    void PreOperationalState::on_reset()
    {
        Serial.println("Reset command, transitioning to Initializaiton state");
        this->context_->transition_to(new InitializationState()); //System reset
    }

    void PreOperationalState::on_op_transition()
    {
        Serial.println("Transition to Operational state");
        this->context_->transition_to(new OperationalState());
    }

    void PreOperationalState::on_pre_transition()
    {

    }

    void PreOperationalState::on_fault()
    {
        Serial.println("Fault detection, transitioning to Stopped state");
        this->context_->transition_to(new StoppedState());
    }

    void PreOperationalState::on_configure()
    {
        Serial.println("You are in the configuration part:");

        Serial.println("Please select a controller type: ");
        Serial.println("Enter '1' for a P controller");
        Serial.println("Enter '2' for a PI controller");
        int controllerType = 0;

        while (true) {
            if (Serial.available()) {
                char incomingChar = Serial.read();
                if (incomingChar == '1') {
                    controllerType = 1; // P controller selected
                    Serial.println("P controller selected");
                    break;
                }
                else if (incomingChar == '2') {
                    controllerType = 2; // PI controller selected
                    Serial.println("PI controller selected");
                    break;
                } else {
                    Serial.println("Invalid input. Please enter '1' or '2'.");
                }
            }
        }

        //context_->setControllerType(controllerType); // IMPLEMENT
        //The next 3 lines are just temporary
        PwmControl* pwmControl = new PwmControl(context_);
        pwmControl->setupPWM_Timer1(); // Set up Timer1 for motor speed control (PWM)
        context_->setPwmControl(pwmControl);

        Serial.println("Enter a value for Kp: ");
        String KpString = ""; // String to store the input

        while (true){
            if (Serial.available()) {
                char incomingChar = Serial.read();
                if (incomingChar == '\n'){ // Enter key pressed
                    break; // Exit loop if Enter is pressed
                }
                if (isDigit(incomingChar) || incomingChar == '.') {
                    KpString += incomingChar; // Append valid characters (digits or a decimal point)
                    Serial.print(incomingChar); // Echo character back to the user
                }
            }
        }
        float Kp = KpString.toFloat(); // Convert the string to a float
        context_->setKp(Kp); // Set the proportional gain for the controller
        Serial.println("\nKp set to: " + String(Kp));

        if (controllerType == 2) {
            Serial.println("Enter a value for Ti: ");
            String TiString = ""; // String to store the input
            while(true) {
                if (Serial.available()) {
                    char incomingChar = Serial.read();
                    if (incomingChar == '\n'){ // Enter key pressed
                        break; // Exit loop if Enter is pressed
                    }
                    if (isDigit(incomingChar) || incomingChar == '.') {
                        TiString += incomingChar; // Append valid characters (digits or a decimal point)
                        Serial.print(incomingChar); // Echo character back to the user
                    }
                }
            }
            float Ti = TiString.toFloat(); // Convert the string to a float
            context_->setTi(Ti); // Set the integral time constant for the controller

            Serial.println("\nTi set to: " + String(Ti));
        } 
        Serial.println("Configuration complete! Available commands:");
        Serial.println("'c' - To Re-Configurate");
        Serial.println("'r' - Reset (transition to Initilaization)");
        Serial.println("'o' - Transition (transition to Operational)");
    }