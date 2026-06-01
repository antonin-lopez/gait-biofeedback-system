#pragma once

#include "AppState.h"
#include "StateMachineInterface.h"
#include "../../include/Types.h"

class Feedback;
class ReposState;
class DiagnosticState;
class CalibrationState;
class CourseNormalState;
class CourseAlerteState;
class PauseState;

// Machine à états du poignet (implémentation concrète).
class StateMachine : public StateMachineInterface {
private:
    AppState* currentState_;

    ReposState& reposState_;
    DiagnosticState& diagnosticState_;
    CalibrationState& calibrationState_;
    CourseNormalState& courseNormalState_;
    CourseAlerteState& courseAlerteState_;
    PauseState& pauseState_;

    bool transitionRequested_;
    AppState* pendingState_;

public:
    StateMachine(ReposState& repos, DiagnosticState& diagnostic, CalibrationState& calibration,
                 CourseNormalState& courseNormal, CourseAlerteState& courseAlerte, PauseState& pause);
    ~StateMachine() = default;

    void requestTransition(AppState* target) override;
    SystemState getCurrentState() const override;

    void update(Feedback& ui, bool btnShort, bool btnLong, float asymmetry);

    static bool isTransitionAllowed(AppState* from, AppState* to);

private:
    void performTransition(AppState* nextState, Feedback& ui);
};
