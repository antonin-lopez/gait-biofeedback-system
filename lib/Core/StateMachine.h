#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "IStateMachine.h"
#include "IState.h"
#include "../../include/Types.h"

class IFeedback;
class ReposState;
class DiagnosticState;
class CalibrationState;
class CourseNormalState;
class CourseAlerteState;
class PauseState;

class StateMachine : public IStateMachine {
private:
    IState* _currentState;

    ReposState* _reposState;
    DiagnosticState* _diagnosticState;
    CalibrationState* _calibrationState;
    CourseNormalState* _courseNormalState;
    CourseAlerteState* _courseAlerteState;
    PauseState* _pauseState;

    bool _transitionRequested;
    SystemState _requestedState;

public:
    StateMachine();
    ~StateMachine();

    void requestTransition(SystemState target) override;
    SystemState getCurrentState() const override;

    void update(IFeedback* ui, bool btnShort, bool btnLong, float asymmetry);

private:
    IState* getStateInstance(SystemState state);
    void performTransition(IState* nextState, IFeedback* ui);
};

#endif // STATEMACHINE_H
