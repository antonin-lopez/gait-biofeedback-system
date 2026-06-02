#pragma once
#include "AppState.h"

class IdleState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::IDLE; }
};

class DiagnosticState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::DIAGNOSTIC; }
};

class CalibrationState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::CALIBRATION; }
};

class RunningNormalState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::RUNNING_NORMAL; }
};

class RunningAlertState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::RUNNING_ALERT; }
};

class PauseState : public AppState
{
public:
    void onEnter(StateMachineInterface *fsm, Feedback &ui) override;
    void execute(StateMachineInterface *fsm, Feedback &ui, const SystemContext &ctx) override;
    void onExit(StateMachineInterface *fsm, Feedback &ui) override;
    SystemState getStateType() const override { return SystemState::PAUSE; }
};