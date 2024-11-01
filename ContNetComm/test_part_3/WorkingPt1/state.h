#ifndef STATE_H
#define STATE_H

#include <Arduino.h>

class Context;

class State
{
protected:
    Context *context_;

public:
    virtual ~State() {}

    void set_context(Context *context)
    {
        this->context_ = context;
    }

    // Virtual methods for each state to implement:
    virtual void on_do() = 0;
    virtual void on_entry() = 0;
    virtual void on_exit() = 0;

    virtual void on_reset() = 0;
    virtual void on_transition() = 0;
    // have one for each action instead of on_eventX()
};

#endif // STATE_H