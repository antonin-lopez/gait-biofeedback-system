#ifndef ISTATEMACHINE_H
#define ISTATEMACHINE_H

#include "../../include/Types.h"

class IStateMachine {
public:
    virtual ~IStateMachine() = default;

    virtual void requestTransition(SystemState target) = 0;
    virtual SystemState getCurrentState() const = 0;
};

#endif // ISTATEMACHINE_H
