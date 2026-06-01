#pragma once

#include "AppState.h"
#include "../../include/Types.h"

class StateMachineInterface;
class Feedback;

// --- REPOS (veille) ---
class ReposState : public AppState {
private:
    AppState* diagnosticTarget_ = nullptr;
    AppState* calibrationTarget_ = nullptr;

public:
    void bindTargets(AppState* diagnostic, AppState* calibration);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::REPOS; }
};

// --- DIAGNOSTIC ---
class DiagnosticState : public AppState {
private:
    AppState* reposTarget_ = nullptr;
    AppState* calibrationTarget_ = nullptr;

public:
    void bindTargets(AppState* repos, AppState* calibration);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::DIAGNOSTIC; }
};

// --- CALIBRATION ---
class CalibrationState : public AppState {
private:
    AppState* reposTarget_ = nullptr;
    AppState* courseNormalTarget_ = nullptr;

public:
    void bindTargets(AppState* repos, AppState* courseNormal);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::CALIBRATION; }
};

// --- COURSE_NORMAL ---
class CourseNormalState : public AppState {
private:
    AppState* pauseTarget_ = nullptr;
    AppState* reposTarget_ = nullptr;

public:
    void bindTargets(AppState* pause, AppState* repos);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::COURSE_NORMAL; }
};

// --- COURSE_ALERTE ---
class CourseAlerteState : public AppState {
private:
    AppState* pauseTarget_ = nullptr;
    AppState* reposTarget_ = nullptr;

public:
    void bindTargets(AppState* pause, AppState* repos);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::COURSE_ALERTE; }
};

// --- PAUSE ---
class PauseState : public AppState {
private:
    AppState* courseNormalTarget_ = nullptr;
    AppState* reposTarget_ = nullptr;

public:
    void bindTargets(AppState* courseNormal, AppState* repos);
    void onEnter(StateMachineInterface* fsm, Feedback& ui) override;
    void execute(StateMachineInterface* fsm, Feedback& ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(StateMachineInterface* fsm, Feedback& ui) override;
    SystemState getStateType() const override { return SystemState::PAUSE; }
};
