#pragma once

#include "../../include/Types.h"

class StateMachineInterface;
class Feedback;

struct SystemContext
{
    bool btnShort;
    bool btnLong;
    float asymmetry;
};

class AppState
{
public:
    virtual ~AppState() = default;
    virtual void onEnter(StateMachineInterface *fsm, Feedback &ui) = 0;
    virtual void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) = 0;
    virtual void onExit(StateMachineInterface *fsm, Feedback &ui) = 0;
    virtual SystemState getStateType() const = 0;
};