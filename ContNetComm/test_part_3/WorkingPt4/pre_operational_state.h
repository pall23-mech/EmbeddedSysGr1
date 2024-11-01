#ifndef PRE_OPERATIONAL_STATE_H
#define PRE_OPERATIONAL_STATE_H

#include <state.h>

class PreOperationalState : public State
{
public:
    void on_entry() override;
    void on_exit() override;
    void on_do() override;
    
    void on_reset() override; 
    void on_pre_transition() override;
    void on_op_transition() override;
    void on_fault() override;
    void on_configure() override;
};

#endif // PRE_OPERATIONAL_STATE_H