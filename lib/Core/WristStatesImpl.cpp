#include "WristStatesImpl.h"
#include "StateMachineInterface.h"
#include "Feedback.h"
#include "../../include/AppConfig.h"

// --- Suppression complète de toutes les anciennes fonctions de type bindTargets() ---

void IdleState::onEnter(StateMachineInterface *, Feedback &ui) { ui.setLedPattern(FeedbackColor::ORANGE_BREATH); }
void IdleState::execute(StateMachineInterface *fsm, Feedback &fire, const SystemContext &ctx)
{
    if (ctx.btnShort)
        fsm->requestTransition(SystemState::DIAGNOSTIC);
    else if (ctx.btnLong)
        fsm->requestTransition(SystemState::CALIBRATION);
}
void IdleState::onExit(StateMachineInterface *, Feedback *) {}

void DiagnosticState::onEnter(StateMachineInterface *, Feedback &ui)
{
    ui.setLedPattern(FeedbackColor::WHITE_FIXED);
    ui.triggerBuzzerBeep(1000, 100);
}
void DiagnosticState::execute(StateMachineInterface *fsm, Feedback &, const SystemContext &ctx)
{
    if (ctx.btnShort)
        fsm->requestTransition(SystemState::IDLE);
    else if (ctx.btnLong)
        fsm->requestTransition(SystemState::CALIBRATION);
}
void DiagnosticState::onExit(StateMachineInterface *, Feedback *) {}

void CalibrationState::onEnter(StateMachineInterface *, Feedback &ui)
{
    ui.setLedPattern(FeedbackColor::BLUE_FLASH);
    ui.triggerBuzzerBeep(1000, 50);
}
void CalibrationState::execute(StateMachineInterface *fsm, Feedback &, const SystemContext &ctx)
{
    if (ctx.btnLong)
        fsm->requestTransition(SystemState::IDLE);
}
void CalibrationState::onExit(StateMachineInterface *, Feedback *) {}

void RunningNormalState::onEnter(StateMachineInterface *, Feedback &ui)
{
    ui.setLedPattern(FeedbackColor::GREEN_FIXED);
    ui.triggerBuzzerBeep(1500, 100);
}
void RunningNormalState::execute(StateMachineInterface *fsm, Feedback &, const SystemContext &ctx)
{
    if (ctx.btnShort)
        fsm->requestTransition(SystemState::PAUSE);
    else if (ctx.btnLong)
        fsm->requestTransition(SystemState::IDLE);
    else if (ctx.asymmetry > ASYMMETRY_THRESHOLD)
        fsm->requestTransition(SystemState::RUNNING_ALERT);
}
void RunningNormalState::onExit(StateMachineInterface *, Feedback *) {}

void RunningAlertState::onEnter(StateMachineInterface *, Feedback &ui)
{
    ui.setLedPattern(FeedbackColor::RED_FLASH);
    ui.triggerBuzzerBeep(2000, 200);
}
void RunningAlertState::execute(StateMachineInterface *fsm, Feedback &, const SystemContext &ctx)
{
    if (ctx.btnShort)
        fsm->requestTransition(SystemState::PAUSE);
    else if (ctx.btnLong)
        fsm->requestTransition(SystemState::IDLE);
    else if (ctx.asymmetry <= (ASYMMETRY_THRESHOLD * ASYMMETRY_HYSTERESIS_RATIO))
        fsm->requestTransition(SystemState::RUNNING_NORMAL);
}
void RunningAlertState::onExit(StateMachineInterface *, Feedback *) {}

void PauseState::onEnter(StateMachineInterface *, Feedback &ui) { ui.setLedPattern(FeedbackColor::ORANGE_FIXED); }
void PauseState::execute(StateMachineInterface *fsm, Feedback &, const SystemContext &ctx)
{
    if (ctx.btnShort)
        fsm->requestTransition(SystemState::RUNNING_NORMAL);
    else if (ctx.btnLong)
        fsm->requestTransition(SystemState::IDLE);
}
void PauseState::onExit(StateMachineInterface *, Feedback *) {}