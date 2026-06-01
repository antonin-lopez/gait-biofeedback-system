#ifndef ISTATE_H
#define ISTATE_H

#include "../../include/Types.h"

class IStateMachine;
class IFeedback;

class IState {
public:
    virtual ~IState() = default;

    virtual void onEnter(IStateMachine* fsm, IFeedback* ui) = 0;
    virtual void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) = 0;
    virtual void onExit(IStateMachine* fsm, IFeedback* ui) = 0;
    virtual SystemState getStateType() const = 0;
};

#endif // ISTATE_H
