#include "WristStatesImpl.h"
#include "StateMachineInterface.h"
#include "Feedback.h"
#include "../../include/AppConfig.h"

void IdleState::bindTargets(AppState* diagnostic, AppState* calibration) {
    diagnosticTarget_ = diagnostic;
    calibrationTarget_ = calibration;
}

void DiagnosticState::bindTargets(AppState* idle, AppState* calibration) {
    idleTarget_ = idle;
    calibrationTarget_ = calibration;
}

void CalibrationState::bindTargets(AppState* idle, AppState* runningNormal) {
    idleTarget_ = idle;
    runningNormalTarget_ = runningNormal;
}

void RunningNormalState::bindTargets(AppState* pause, AppState* idle, AppState* runningAlert) {
    pauseTarget_ = pause;
    idleTarget_ = idle;
    runningAlertTarget_ = runningAlert;
}

void RunningAlertState::bindTargets(AppState* pause, AppState* idle, AppState* runningNormal) {
    pauseTarget_ = pause;
    idleTarget_ = idle;
    runningNormalTarget_ = runningNormal;
}

void PauseState::bindTargets(AppState* runningNormal, AppState* idle) {
    runningNormalTarget_ = runningNormal;
    idleTarget_ = idle;
}

void IdleState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::ORANGE_BREATH);
}

void IdleState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && diagnosticTarget_) {
        fsm->requestTransition(diagnosticTarget_);
    } else if (btnLong && calibrationTarget_) {
        fsm->requestTransition(calibrationTarget_);
    }
}

void IdleState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

void DiagnosticState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::WHITE_FIXED);
    ui.triggerBuzzerBeep(1000, 100);
}

void DiagnosticState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                             float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && idleTarget_) {
        fsm->requestTransition(idleTarget_);
    } else if (btnLong && calibrationTarget_) {
        fsm->requestTransition(calibrationTarget_);
    }
}

void DiagnosticState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

void CalibrationState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::BLUE_FLASH);
    ui.triggerBuzzerBeep(1000, 50);
    ui.triggerBuzzerBeep(1000, 50);
}

void CalibrationState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                              float asymmetry) {
    (void)ui;
    (void)btnShort;
    (void)asymmetry;
    if (btnLong && idleTarget_) {
        fsm->requestTransition(idleTarget_);
    }
}

void CalibrationState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

void RunningNormalState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::GREEN_FIXED);
    ui.triggerBuzzerBeep(1500, 100);
}

void RunningNormalState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                                float asymmetry) {
    (void)ui;
    if (btnShort && pauseTarget_) {
        fsm->requestTransition(pauseTarget_);
    } else if (btnLong && idleTarget_) {
        fsm->requestTransition(idleTarget_);
    } else if (asymmetry > ASYMMETRY_THRESHOLD && runningAlertTarget_) {
        fsm->requestTransition(runningAlertTarget_);
    }
}

void RunningNormalState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

void RunningAlertState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::RED_FLASH);
    ui.triggerBuzzerBeep(2000, 200);
}

void RunningAlertState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                                float asymmetry) {
    (void)ui;
    if (btnShort && pauseTarget_) {
        fsm->requestTransition(pauseTarget_);
    } else if (btnLong && idleTarget_) {
        fsm->requestTransition(idleTarget_);
    } else if (asymmetry <= (ASYMMETRY_THRESHOLD * ASYMMETRY_HYSTERESIS_RATIO) && runningNormalTarget_) {
        fsm->requestTransition(runningNormalTarget_);
    }
}

void RunningAlertState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

void PauseState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::ORANGE_FIXED);
}

void PauseState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && runningNormalTarget_) {
        fsm->requestTransition(runningNormalTarget_);
    } else if (btnLong && idleTarget_) {
        fsm->requestTransition(idleTarget_);
    }
}

void PauseState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}
