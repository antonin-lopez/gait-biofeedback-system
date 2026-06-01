#pragma once

#include "AppState.h"
#include "../../include/Types.h"

class StateMachineInterface;
class Feedback;

// --- IDLE (veille) ---
class IdleState : public AppState {
private:
    AppState* diagnosticTarget_ = nullptr;
    AppState* calibrationTarget_ = nullptr;

public:
    void bindTargets(AppState* diagnostic, AppState* calibration);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::IDLE; }
};

// --- DIAGNOSTIC ---
class DiagnosticState : public AppState {
private:
    AppState* idleTarget_ = nullptr;
    AppState* calibrationTarget_ = nullptr;

public:
    void bindTargets(AppState* idle, AppState* calibration);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::DIAGNOSTIC; }
};

// --- CALIBRATION ---
class CalibrationState : public AppState {
private:
    AppState* idleTarget_ = nullptr;
    AppState* runningNormalTarget_ = nullptr;

public:
    void bindTargets(AppState* idle, AppState* runningNormal);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::CALIBRATION; }
};

// --- RUNNING_NORMAL ---
class RunningNormalState : public AppState {
private:
    AppState* pauseTarget_ = nullptr;
    AppState* idleTarget_ = nullptr;
    AppState* runningAlertTarget_ = nullptr;

public:
    void bindTargets(AppState* pause, AppState* idle, AppState* runningAlert);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::RUNNING_NORMAL; }
};

// --- RUNNING_ALERT ---
class RunningAlertState : public AppState {
private:
    AppState* pauseTarget_ = nullptr;
    AppState* idleTarget_ = nullptr;
    AppState* runningNormalTarget_ = nullptr;

public:
    void bindTargets(AppState* pause, AppState* idle, AppState* runningNormal);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::RUNNING_ALERT; }
};

// --- PAUSE ---
class PauseState : public AppState {
private:
    AppState* runningNormalTarget_ = nullptr;
    AppState* idleTarget_ = nullptr;

public:
    void bindTargets(AppState* runningNormal, AppState* idle);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::PAUSE; }
};
