#include "WristStatesImpl.h"
#include "StateMachineInterface.h"
#include "../../lib/HAL/Feedback.h"
#include "../../include/AppConfig.h"

void ReposState::bindTargets(AppState* diagnostic, AppState* calibration) {
    diagnosticTarget_ = diagnostic;
    calibrationTarget_ = calibration;
}

void DiagnosticState::bindTargets(AppState* repos, AppState* calibration) {
    reposTarget_ = repos;
    calibrationTarget_ = calibration;
}

void CalibrationState::bindTargets(AppState* repos, AppState* courseNormal) {
    reposTarget_ = repos;
    courseNormalTarget_ = courseNormal;
}

void CourseNormalState::bindTargets(AppState* pause, AppState* repos) {
    pauseTarget_ = pause;
    reposTarget_ = repos;
}

void CourseAlerteState::bindTargets(AppState* pause, AppState* repos) {
    pauseTarget_ = pause;
    reposTarget_ = repos;
}

void PauseState::bindTargets(AppState* courseNormal, AppState* repos) {
    courseNormalTarget_ = courseNormal;
    reposTarget_ = repos;
}

// --- REPOS (veille) ---
void ReposState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::ORANGE_BREATH);
}

void ReposState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && diagnosticTarget_) {
        fsm->requestTransition(diagnosticTarget_);
    } else if (btnLong && calibrationTarget_) {
        fsm->requestTransition(calibrationTarget_);
    }
}

void ReposState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

// --- DIAGNOSTIC ---
void DiagnosticState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::WHITE_FIXED);
    ui.triggerBuzzerBeep(1000, 100);
}

void DiagnosticState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                             float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && reposTarget_) {
        fsm->requestTransition(reposTarget_);
    } else if (btnLong && calibrationTarget_) {
        fsm->requestTransition(calibrationTarget_);
    }
}

void DiagnosticState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

// --- CALIBRATION ---
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
    if (btnLong && reposTarget_) {
        fsm->requestTransition(reposTarget_);
    }
}

void CalibrationState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

// --- COURSE_NORMAL ---
void CourseNormalState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::GREEN_FIXED);
    ui.triggerBuzzerBeep(1500, 100);
}

void CourseNormalState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                                float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && pauseTarget_) {
        fsm->requestTransition(pauseTarget_);
    } else if (btnLong && reposTarget_) {
        fsm->requestTransition(reposTarget_);
    }
}

void CourseNormalState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

// --- COURSE_ALERTE ---
void CourseAlerteState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::RED_FLASH);
    ui.triggerBuzzerBeep(2000, 200);
}

void CourseAlerteState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong,
                                float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && pauseTarget_) {
        fsm->requestTransition(pauseTarget_);
    } else if (btnLong && reposTarget_) {
        fsm->requestTransition(reposTarget_);
    }
}

void CourseAlerteState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}

// --- PAUSE ---
void PauseState::onEnter(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    ui.setLedPattern(FeedbackColor::ORANGE_FIXED);
}

void PauseState::execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) {
    (void)ui;
    (void)asymmetry;
    if (btnShort && courseNormalTarget_) {
        fsm->requestTransition(courseNormalTarget_);
    } else if (btnLong && reposTarget_) {
        fsm->requestTransition(reposTarget_);
    }
}

void PauseState::onExit(StateMachineInterface* fsm, Feedback& ui) {
    (void)fsm;
    (void)ui;
}
