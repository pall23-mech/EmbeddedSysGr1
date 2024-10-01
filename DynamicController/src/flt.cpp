// Definition of states
enum State {
    INIT,
    PRE_OPERATIONAL,
    OPERATIONAL,
    STOPPED
};

// Initial state
State currentState = INIT;

// Pin definitions for LED and FLT (fault signal)
const int LED_PIN = LED_BUILTIN; // Built-in LED on the Arduino board
const int FLT_PIN = 2; // Pin for fault signal (modify according to your configuration)

void setup() {
    Serial.begin(9600); // Start serial communication
    pinMode(LED_PIN, OUTPUT); // Set LED pin as output
    pinMode(FLT_PIN, INPUT); // Set FLT pin as input
    digitalWrite(LED_PIN, LOW); // Ensure the LED is off at the start
}

void loop() {
    switch (currentState) {
        case INIT:
            Serial.println("Initializing...");
            // Initialize motor settings
            initializeMotor(); // Custom function to initialize the motor
            delay(1000); // Wait to simulate initialization
            currentState = PRE_OPERATIONAL; // Transition to Pre-operational state
            break;

        case PRE_OPERATIONAL:
            Serial.println("Pre-operational State");
            blinkLED(1); // Blink LED at 1 Hz
            if (Serial.available()) {
                char command = Serial.read();
                if (command == 'o') {
                    currentState = OPERATIONAL; // Command to transition to Operational state
                    Serial.println("Transitioning to Operational State");
                }
                if (command == 'r') {
                    currentState = INIT; // Command to reset
                    Serial.println("Transitioning to Initialization State");
                }
            }
            break;

        case OPERATIONAL:
            Serial.println("Operational State");
            digitalWrite(LED_PIN, HIGH); // LED remains continuously on

            // Fault detection
            if (digitalRead(FLT_PIN) == HIGH) {
                Serial.println("Fault detected! Transitioning to Stopped State");
                currentState = STOPPED; // If a fault is detected, transition to Stopped state
            }

            if (Serial.available()) {
                char command = Serial.read();
                if (command == 'p') {
                    currentState = PRE_OPERATIONAL; // Transition back to Pre-operational
                    Serial.println("Transitioning to Pre-operational State");
                }
                if (command == 'r') {
                    currentState = INIT; // Reset
                    Serial.println("Transitioning to Initialization State");
                }
            }
            break;

        case STOPPED:
            Serial.println("Stopped State");
            blinkLED(2); // Blink LED at 2 Hz
            if (Serial.available()) {
                char command = Serial.read();
                if (command == 'o') {
                    currentState = OPERATIONAL; // Transition back to Operational
                    Serial.println("Transitioning to Operational State");
                }
                if (command == 'r') {
                    currentState = INIT; // Reset
                    Serial.println("Transitioning to Initialization State");
                }
                if (command == 'p') {
                    currentState = PRE_OPERATIONAL; // Transition back to Pre-operational
                    Serial.println("Transitioning to Pre-operational State");
                }
            }
            break;
    }
}

// Function to initialize the motor (to be implemented)
void initializeMotor() {
    // Add motor initialization instructions here
}

// Function to blink the LED
void blinkLED(int frequency) {
    digitalWrite(LED_PIN, HIGH);
    delay(500 / frequency);
    digitalWrite(LED_PIN, LOW);
    delay(500 / frequency);
}
