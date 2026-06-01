#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "../../include/AppConfig.h"

StateMachine::StateMachine(ReposState& repos, DiagnosticState& diagnostic, CalibrationState& calibration,
                           CourseNormalState& courseNormal, CourseAlerteState& courseAlerte, PauseState& pause)
    : _reposState(repos), _diagnosticState(diagnostic), _calibrationState(calibration),
      _courseNormalState(courseNormal), _courseAlerteState(courseAlerte), _pauseState(pause),
      _transitionRequested(false), _requestedState(SystemState::REPOS) {
    _currentState = &_reposState;
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
            return &_reposState;
        case SystemState::DIAGNOSTIC:
            return &_diagnosticState;
        case SystemState::CALIBRATION:
            return &_calibrationState;
        case SystemState::COURSE_NORMAL:
            return &_courseNormalState;
        case SystemState::COURSE_ALERTE:
            return &_courseAlerteState;
        case SystemState::PAUSE:
            return &_pauseState;
        default:
            return &_reposState;
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

