#include "WristStatesImpl.h"
#include "IStateMachine.h"
#include "../../lib/HAL/IFeedback.h"
#include "../../include/AppConfig.h"

// ============ REPOS (Veille) ============
void ReposState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::ORANGE_BREATH);
    }
}

void ReposState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnShort) {
        fsm->requestTransition(SystemState::DIAGNOSTIC);
    } else if (btnLong) {
        fsm->requestTransition(SystemState::CALIBRATION);
    }
}

void ReposState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}

// ============ DIAGNOSTIC ============
void DiagnosticState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::WHITE_FIXED);
        ui->triggerBuzzerBeep(1000, 100);
    }
}

void DiagnosticState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnShort) {
        fsm->requestTransition(SystemState::REPOS);
    } else if (btnLong) {
        fsm->requestTransition(SystemState::CALIBRATION);
    }
}

void DiagnosticState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}

// ============ CALIBRATION ============
void CalibrationState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::BLUE_FLASH);
        ui->triggerBuzzerBeep(1000, 50);
        ui->triggerBuzzerBeep(1000, 50);
    }
}

void CalibrationState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnLong) {
        fsm->requestTransition(SystemState::REPOS);
    }
}

void CalibrationState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}

// ============ COURSE_NORMAL ============
void CourseNormalState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::GREEN_FIXED);
        ui->triggerBuzzerBeep(1500, 100);
    }
}

void CourseNormalState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnShort) {
        fsm->requestTransition(SystemState::PAUSE);
    } else if (btnLong) {
        fsm->requestTransition(SystemState::REPOS);
    }
}

void CourseNormalState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}

// ============ COURSE_ALERTE ============
void CourseAlerteState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::RED_FLASH);
        ui->triggerBuzzerBeep(2000, 200);
    }
}

void CourseAlerteState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnShort) {
        fsm->requestTransition(SystemState::PAUSE);
    } else if (btnLong) {
        fsm->requestTransition(SystemState::REPOS);
    }
}

void CourseAlerteState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}

// ============ PAUSE ============
void PauseState::onEnter(IStateMachine* fsm, IFeedback* ui) {
    if (ui) {
        ui->setLedPattern(FeedbackColor::ORANGE_FIXED);
    }
}

void PauseState::execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) {
    if (btnShort) {
        fsm->requestTransition(SystemState::COURSE_NORMAL);
    } else if (btnLong) {
        fsm->requestTransition(SystemState::REPOS);
    }
}

void PauseState::onExit(IStateMachine* fsm, IFeedback* ui) {
    // Pas de nettoyage spécifique
}
