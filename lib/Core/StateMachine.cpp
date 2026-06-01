#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "../../include/AppConfig.h"

StateMachine::StateMachine()
    : _transitionRequested(false), _requestedState(SystemState::REPOS) {
    _reposState = new ReposState();
    _diagnosticState = new DiagnosticState();
    _calibrationState = new CalibrationState();
    _courseNormalState = new CourseNormalState();
    _courseAlerteState = new CourseAlerteState();
    _pauseState = new PauseState();

    _currentState = _reposState;
}

StateMachine::~StateMachine() {
    delete _reposState;
    delete _diagnosticState;
    delete _calibrationState;
    delete _courseNormalState;
    delete _courseAlerteState;
    delete _pauseState;
}

void StateMachine::requestTransition(SystemState target) {
    _transitionRequested = true;
    _requestedState = target;
}

SystemState StateMachine::getCurrentState() const {
    return _currentState ? _currentState->getStateType() : SystemState::REPOS;
}

IState* StateMachine::getStateInstance(SystemState state) {
    switch (state) {
        case SystemState::REPOS:
            return _reposState;
        case SystemState::DIAGNOSTIC:
            return _diagnosticState;
        case SystemState::CALIBRATION:
            return _calibrationState;
        case SystemState::COURSE_NORMAL:
            return _courseNormalState;
        case SystemState::COURSE_ALERTE:
            return _courseAlerteState;
        case SystemState::PAUSE:
            return _pauseState;
        default:
            return _reposState;
    }
}

void StateMachine::performTransition(IState* nextState, IFeedback* ui) {
    if (!nextState || nextState == _currentState) return;

    if (_currentState) {
        _currentState->onExit(this, ui);
    }

    _currentState = nextState;
    _currentState->onEnter(this, ui);
}

void StateMachine::update(IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (!_currentState) return;

    _currentState->execute(this, ui, btnShort, btnLong, asymmetry);

    if (_transitionRequested) {
        IState* nextState = getStateInstance(_requestedState);
        performTransition(nextState, ui);
        _transitionRequested = false;
    }
}
