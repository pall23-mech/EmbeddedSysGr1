#include <Arduino.h>
#include "digital_in.h"
#include "digital_out.h"
#include "encoder.h"
#include <util/delay.h>
#include "timer_msec.h"
#include "analog_out.h"
#include "PI_controller.h"

// Forward declaration of Context to allow State to reference it
class Context;

class State {
protected:
    Context *context_;

public:
    virtual ~State() {}
    void set_context(Context *context) { this->context_ = context; }
    virtual void on_do() = 0;
    virtual void on_entry() = 0;
    virtual void on_exit() = 0;
    virtual void on_event1() = 0;
    virtual void on_event2() = 0;
};

// Context class that manages state transitions
class Context {
private:
    State *state_;

public:
    Context(State *state) : state_(nullptr) {
        this->transition_to(state);
    }

    ~Context() { delete state_; }

    void transition_to(State *state) {
        if (this->state_ != nullptr) {
            this->state_->on_exit();
            delete this->state_;
        }
        this->state_ = state;
        this->state_->set_context(this);
        this->state_->on_entry();
    }

    void do_work() { this->state_->on_do(); }
    void event1() { this->state_->on_event1(); }
    void event2() { this->state_->on_event2(); }
};

// Timer and motor control variables
Timer_msec timer;
Digital_out motorIN2(0); // pin D8 for motor control
Analog_out motorIN1(1);  // PWM pin D9 for motor control
Encoder encoder(3, 4);   // Encoder pins D11, D12
float kp = 0.01;
float ti = 5;
float MIN_PWM_THRESHOLD = 0.1;
PI_controller controller(kp, ti);
unsigned long lastPrintTime = 0;
double actualSpeed = 0;
double refSpeed = 0;
double pwmValue = 0;
double controlSignal = 0;

// Define RunningState first
class RunningState : public State {
public:
    void on_do() override {
        Serial.println("Motor is running.");
        actualSpeed = abs(encoder.speed());
        controlSignal = controller.update(refSpeed, actualSpeed);
        controlSignal = constrain(controlSignal, MIN_PWM_THRESHOLD, 1.0);
        pwmValue = controlSignal;
        motorIN1.set(pwmValue);
    }

    void on_entry() override {
        Serial.println("Entering Running State.");
        motorIN2.set_hi(); // Enable motor
    }

    void on_exit() override {
        Serial.println("Exiting Running State.");
        motorIN1.set(0); // Stop PWM output
    }

    void on_event1() override {}

    void on_event2() override {
        this->context_->transition_to(new StoppedState);
    }
};

// Define StoppedState after RunningState
class StoppedState : public State {
public:
    void on_do() override {
        Serial.println("Motor is stopped.");
    }

    void on_entry() override {
        Serial.println("Entering Stopped State.");
        motorIN2.set_lo(); // Disable motor
        motorIN1.set(0);   // Set PWM to 0
    }

    void on_exit() override {
        Serial.println("Exiting Stopped State.");
    }

    void on_event1() override {
        this->context_->transition_to(new RunningState);
    }

    void on_event2() override {}
};

// Define PreOperationalState
class PreOperationalState : public State {
public:
    void on_do() override {
        Serial.println("Motor is in pre-operational state.");
    }

    void on_entry() override {
        Serial.println("Entering Pre-Operational State.");
        motorIN1.set(0.2); // Example low PWM
        motorIN2.set_hi();
    }

    void on_exit() override {
        Serial.println("Exiting Pre-Operational State.");
        motorIN1.set(0);
    }

    void on_event1() override {
        this->context_->transition_to(new StoppedState);
    }

    void on_event2() override {
        this->context_->transition_to(new RunningState);
    }
};

Context *motorContext;

void setup() {
    Serial.begin(115200);
    motorContext = new Context(new StoppedState);
    timer.init(0.1);  // Timer interval in ms
    sei();            // Enable global interrupts

    encoder.init();
    motorIN2.init();
    motorIN1.init(10);  // PWM frequency in ms
    motorIN1.set(0);    // Initialize PWM to 0
}

void loop() {
    uint8_t msg[8];

    // Check for incoming messages to transition states
    if (Serial.available() >= 8) {
        Serial.readBytes(msg, 8);

        // Process command for specific states
        if (msg[0] == 0x00 && msg[1] == 0x06) {
            switch (msg[4]) {
                case 0x01:
                    motorContext->event1(); // Transition to operational (RUNNING)
                    Serial.println("Set node operational: RUNNING");
                    break;
                case 0x02:
                    motorContext->event2(); // Transition to stopped (STOPPED)
                    Serial.println("Stop node: STOPPED");
                    break;
                case 0x80:
                    motorContext->transition_to(new PreOperationalState);
                    Serial.println("Set node pre-operational: PRE_OPERATIONAL");
                    break;
                case 0x81:
                    motorContext->transition_to(new StoppedState);
                    Serial.println("Reset node: STOPPED");
                    break;
                case 0x82:
                    motorContext->transition_to(new StoppedState); // Reset communication
                    refSpeed = 0;
                    Serial.println("Reset communications: STOPPED and reset");
                    break;
                default:
                    Serial.println("Unknown command");
                    break;
            }
        }
    }

    motorContext->do_work(); // Execute the current state's main action
    delay(100); // Simulate some processing time
}
