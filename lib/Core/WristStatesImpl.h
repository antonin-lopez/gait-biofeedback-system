#ifndef WRIST_STATES_IMPL_H
#define WRIST_STATES_IMPL_H

#include "IState.h"
#include "../../include/Types.h"

class IStateMachine;
class IFeedback;

// ============ REPOS (Veille) ============
class ReposState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::REPOS; }
};

// ============ DIAGNOSTIC ============
class DiagnosticState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::DIAGNOSTIC; }
};

// ============ CALIBRATION ============
class CalibrationState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::CALIBRATION; }
};

// ============ COURSE_NORMAL ============
class CourseNormalState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::COURSE_NORMAL; }
};

// ============ COURSE_ALERTE ============
class CourseAlerteState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::COURSE_ALERTE; }
};

// ============ PAUSE ============
class PauseState : public IState {
public:
    void onEnter(IStateMachine* fsm, IFeedback* ui) override;
    void execute(IStateMachine* fsm, IFeedback* ui, bool btnShort, bool btnLong, float asymmetry) override;
    void onExit(IStateMachine* fsm, IFeedback* ui) override;
    SystemState getStateType() const override { return SystemState::PAUSE; }
};

#endif // WRIST_STATES_IMPL_H
