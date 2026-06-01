#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "../../include/AppConfig.h"
#include "../../lib/HAL/Feedback.h"

StateMachine::StateMachine(ReposState& repos, DiagnosticState& diagnostic, CalibrationState& calibration,
                           CourseNormalState& courseNormal, CourseAlerteState& courseAlerte, PauseState& pause)
    : reposState_(repos),
      diagnosticState_(diagnostic),
      calibrationState_(calibration),
      courseNormalState_(courseNormal),
      courseAlerteState_(courseAlerte),
      pauseState_(pause),
      transitionRequested_(false),
      pendingState_(nullptr) {
    currentState_ = &reposState_;
}

void StateMachine::requestTransition(AppState* target) {
    if (!target || !isTransitionAllowed(currentState_, target)) {
        return;
    }

    transitionRequested_ = true;
    pendingState_ = target;
}

SystemState StateMachine::getCurrentState() const {
    return currentState_ ? currentState_->getStateType() : SystemState::REPOS;
}

bool StateMachine::isTransitionAllowed(AppState* from, AppState* to) {
    if (!from || !to || from == to) {
        return false;
    }

    const SystemState fromState = from->getStateType();
    const SystemState toState = to->getStateType();

    switch (fromState) {
        case SystemState::REPOS:
            return toState == SystemState::DIAGNOSTIC || toState == SystemState::CALIBRATION;

        case SystemState::DIAGNOSTIC:
            return toState == SystemState::REPOS || toState == SystemState::CALIBRATION;

        case SystemState::CALIBRATION:
            return toState == SystemState::REPOS || toState == SystemState::COURSE_NORMAL;

        case SystemState::COURSE_NORMAL:
            return toState == SystemState::PAUSE || toState == SystemState::REPOS ||
                   toState == SystemState::COURSE_ALERTE;

        case SystemState::COURSE_ALERTE:
            return toState == SystemState::PAUSE || toState == SystemState::REPOS ||
                   toState == SystemState::COURSE_NORMAL;

        case SystemState::PAUSE:
            return toState == SystemState::COURSE_NORMAL || toState == SystemState::REPOS;

        default:
            return false;
    }
}

void StateMachine::performTransition(AppState* nextState, Feedback& ui) {
    if (!nextState || nextState == currentState_) {
        return;
    }

    if (currentState_) {
        currentState_->onExit(this, ui);
    }

    currentState_ = nextState;
    currentState_->onEnter(this, ui);
}

void StateMachine::update(Feedback& ui, bool btnShort, bool btnLong, float asymmetry) {
    if (!currentState_) {
        return;
    }

    currentState_->execute(this, ui, btnShort, btnLong, asymmetry);

    if (transitionRequested_ && pendingState_) {
        performTransition(pendingState_, ui);
        transitionRequested_ = false;
        pendingState_ = nullptr;
    }
}
