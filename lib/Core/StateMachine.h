#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "../../include/Types.h"

class StateMachine {
private:
    SystemState _currentState;

public:
    StateMachine();
    void transitionTo(SystemState newState);
    SystemState getCurrentState() const;
};

#endif // STATEMACHINE_H

