#ifndef STOPPED_STATE_H
#define STOPPED_STATE_H

#include <state.h>

class StoppedState : public State
{
public:
    void on_entry() override;
    void on_exit() override;
    void on_do() override;

    // change on_eventX to our event names
    void on_reset() override; 
    void on_pre_transition() override;
    void on_op_transition() override; 
    void on_fault() override;
    void on_configure() override;
};

#endif // STOPPED_STATE_H