#pragma once
#include "State.h"  // Include the full definition of State
#include <Arduino.h>

class Context {
private:
    State *state_;  // Pointer to the current state of the context

public:
    Context(State *state = nullptr) : state_(nullptr) {
        this->transition_to(state);
    }

    ~Context();

    // Allow changing the State object at runtime
    void transition_to(State *state);

    // Delegate behavior to the current State object
    void do_work();
    void command_go();
    void command_stop();
    void timeout();
    void on_operational();
};

#endif // CONTEXT_H
