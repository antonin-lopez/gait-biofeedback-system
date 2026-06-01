#include "StateMachine.h"

StateMachine::StateMachine() : _currentState(SystemState::REPOS) {}

void StateMachine::transitionTo(SystemState newState) {
    _currentState = newState;
}

SystemState StateMachine::getCurrentState() const {
    return _currentState;
}
