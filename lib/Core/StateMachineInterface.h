#pragma once

#include "../../include/Types.h"

class StateMachineInterface
{
public:
    virtual ~StateMachineInterface() = default;
    virtual void requestTransition(SystemState targetState) = 0;
    virtual SystemState getCurrentState() const = 0;
};