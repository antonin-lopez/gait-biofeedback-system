#pragma once

#include "AppState.h"
#include "StateMachineInterface.h"
#include "../../include/Types.h"

class Feedback;
class IdleState;
class DiagnosticState;
class CalibrationState;
class RunningNormalState;
class RunningAlertState;
class PauseState;

// Machine à états du poignet (implémentation concrète).
class StateMachine : public StateMachineInterface {
private:
    AppState* currentState_;

    IdleState& idleState_;
    DiagnosticState& diagnosticState_;
    CalibrationState& calibrationState_;
    RunningNormalState& runningNormalState_;
    RunningAlertState& runningAlertState_;
    PauseState& pauseState_;

    bool transitionRequested_;
    AppState* pendingState_;

public:
    StateMachine(IdleState& idle, DiagnosticState& diagnostic, CalibrationState& calibration,
                 RunningNormalState& runningNormal, RunningAlertState& runningAlert, PauseState& pause,
                 Feedback& ui);
    ~StateMachine() = default;

    void requestTransition(AppState* target) override;
    void forceTransition(AppState* target, Feedback& ui);
    SystemState getCurrentState() const override;

    void update(Feedback& ui, bool btnShort, bool btnLong, float asymmetry);

private:
    void performTransition(AppState* nextState, Feedback& ui);
};
