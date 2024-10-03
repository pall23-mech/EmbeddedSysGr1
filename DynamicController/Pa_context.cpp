#include <context.h>
#include <state.h>
#include <encoder.h>
#include <pwm_control.h>

// Define the extern varialbes to match their usage in pwm_control.cpp
Encoder encoder(16, 17, 1400.0);  // Define encoder instance
float targetPPS = 2800.0;            // Define targetPPS
unsigned long lastControlUpdate = 0; // Define lastControlUpdate

Context::Context(State *state) : state_(nullptr), encoder(16, 17, 1400.0), directionControl(8)
{
    // Initialize state and set the context
    this->transition_to(state);
}

Context::~Context()
{
    delete state_;
}

// Transition to a new state
void Context::transition_to(State *state)
{
    if (this->state_ != nullptr)
    {
        this->state_->on_exit();
        delete this->state_;
    }

    this->state_ = state;
    this->state_->set_context(this);
    this->state_->on_entry();
}

// Execute the current state's behaviour
void Context::do_work()
{
    this->state_->on_do();
}

// Switch out event1.. with our event names

// Handle a reset command
void Context::reset()
{
    this->state_->on_reset();
}

// Placeholder for a generic transition logic
void Context::transition()
{
    this->state_->on_transition();
}

// Handle a fault detection
void Context::fault()
{
    this->state_->on_fault();
}