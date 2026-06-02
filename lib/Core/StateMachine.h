#pragma once

#include "AppState.h"
#include "StateMachineInterface.h"
#include "../../include/Types.h"

class Feedback;

class StateMachine : public StateMachineInterface
{
private:
    AppState *currentState_ = nullptr;
    AppState *states_[static_cast<uint8_t>(SystemState::COUNT)] = {nullptr};
    bool transitionRequested_ = false;
    SystemState pendingState_ = SystemState::IDLE;

public:
    StateMachine() = default;
    ~StateMachine() = default;

    void registerState(AppState *state);
    void setInitialState(SystemState state, Feedback &ui);

    void requestTransition(SystemState targetState) override;
    SystemState getCurrentState() const override;
    void update(Feedback &ui, const SystemContext &ctx);

private:
    void performTransition(SystemState nextState, Feedback &ui);
};