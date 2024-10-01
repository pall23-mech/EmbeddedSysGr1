#ifndef OPERATIONAL_STATE_H
#define OPERATIONAL_STATE_H

#include <state.h>

class OperationalState : public State
{
public:
    void on_entry() override;
    void on_exit() override;
    void on_do() override;
    
    void on_reset() override; 
    void on_transition() override;
    void on_fault() override;
};

#endif // OPERATIONAL_STATE_H