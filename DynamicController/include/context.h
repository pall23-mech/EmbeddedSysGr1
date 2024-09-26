#ifndef CONTEXT_H
#define CONTEXT_H

class State;

class Context
{
private:
    State *state_;

public:
    Context(State *state);
    ~Context();

    void transition_to(State *state);
    void do_work();

    void reset();
    void transition(); // change later...
};

#endif // CONTEXT_H