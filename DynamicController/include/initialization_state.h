#ifndef INITIALIZATION_STATE_H
#define INITIALIZATION_STATE_H

#include <state.h>

class InitializationState : public State
{
public:
    void on_entry() override;
    void on_exit() override;
    void on_do() override;
    void on_event1() override; 
    void on_event2() override; 
};

#endif // INITIALIZATION_STATE_H