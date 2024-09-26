void InitializationState::on_entry() {
    Serial.println("Entering Initialization State...");
    // Perform initialization tasks (motor setup, sensor checks, communication setup)
    motor_speed_setpoint = 0; // Example parameter initialization
    pwm_signal = 0; // Example PWM signal setup
    // Other initialization tasks
}

void InitializationState::on_exit() {
    Serial.println("Exiting Initialization State...");
    // Any cleanup tasks needed when leaving initialization
}

void InitializationState::on_do() {
    // Tasks to perform while in Initialization state
    Serial.println("Initializing...");
    // Once initialization is done, automatically transition to Pre-operational state
    context_->transition_to(new PreOperationalState());
}

void InitializationState::on_event1() {
    // Handle specific event in this state (not used in this example)
}

void InitializationState::on_event2() {
    // Handle specific event in this state (not used in this example)
}