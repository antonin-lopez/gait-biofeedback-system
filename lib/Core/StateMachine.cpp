#include "StateMachine.h"
#include "Feedback.h"

void StateMachine::registerState(AppState *state)
{
    if (state)
    {
        states_[static_cast<uint8_t>(state->getStateType())] = state;
    }
}

void StateMachine::setInitialState(SystemState state, Feedback &ui)
{
    currentState_ = states_[static_cast<uint8_t>(state)];
    if (currentState_)
    {
        currentState_->onEnter(this, ui);
    }
}

void StateMachine::requestTransition(SystemState targetState)
{
    transitionRequested_ = true;
    pendingState_ = targetState;
}

SystemState StateMachine::getCurrentState() const
{
    return currentState_ ? currentState_->getStateType() : SystemState::IDLE;
}

void StateMachine::performTransition(SystemState nextState, Feedback &ui)
{
    AppState *target = states_[static_cast<uint8_t>(nextState)];
    if (!target || target == currentState_)
        return;

    if (currentState_)
    {
        currentState_->onExit(this, ui);
    }
    currentState_ = target;
    currentState_->onEnter(this, ui);
}

void StateMachine::update(Feedback &ui, const SystemContext &ctx)
{
    if (!currentState_)
        return;

    currentState_->execute(this, ui, ctx);

    if (transitionRequested_)
    {
        performTransition(pendingState_, ui);
        transitionRequested_ = false;
    }
}