#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "../../include/AppConfig.h"
#include "Feedback.h"

StateMachine::StateMachine(IdleState& idle, DiagnosticState& diagnostic, CalibrationState& calibration,
                           RunningNormalState& runningNormal, RunningAlertState& runningAlert, PauseState& pause,
                           Feedback& ui)
    : idleState_(idle),
      diagnosticState_(diagnostic),
      calibrationState_(calibration),
      runningNormalState_(runningNormal),
      runningAlertState_(runningAlert),
      pauseState_(pause),
      transitionRequested_(false),
      pendingState_(nullptr) {
    currentState_ = &idleState_;
    currentState_->onEnter(this, ui);
}

void StateMachine::requestTransition(AppState* target) {
    if (!target || target == currentState_) {
        return;
    }

    transitionRequested_ = true;
    pendingState_ = target;
}

void StateMachine::forceTransition(AppState* target, Feedback& ui) {
    if (!target) {
        return;
    }
    performTransition(target, ui);
}

SystemState StateMachine::getCurrentState() const {
    return currentState_ ? currentState_->getStateType() : SystemState::IDLE;
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
