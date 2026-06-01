#pragma once

#include "../../include/Types.h"

class StateMachineInterface;
class Feedback;

// État abstrait de la machine à états du poignet.
class AppState {
public:
    virtual ~AppState() = default;

    virtual void onEnter(StateMachineInterface* fsm, Feedback& ui) = 0;
    virtual void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                         float asymmetry) = 0;
    virtual void onExit(StateMachineInterface* fsm, Feedback& ui) = 0;
    virtual SystemState getStateType() const = 0;
};
