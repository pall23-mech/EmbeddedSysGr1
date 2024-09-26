#include <context.h>
#include <state.h>

Context::Context(State *state) : state_(nullptr)
{
    this->transition_to(state);
}

Context::~Context()
{
    delete state_;
}

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

void Context::do_work()
{
    this->state_->on_do();
}

// Switch out event1.. with our event names

void Context::reset()
{
    this->state_->on_reset();
}

void Context::event2()
{
    this->state_->on_transition();
}