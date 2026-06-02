#include <unity.h>
#include "../../../lib/Core/StateMachine.h"
#include "../../../lib/Core/WristStatesImpl.h"
#include "../../../lib/HAL/Mock/MockFeedback.h"
#include "../../../include/Types.h"

static void bindAllStates(IdleState& idle, DiagnosticState& diagnostic, CalibrationState& calibration,
                          RunningNormalState& runningNormal, RunningAlertState& runningAlert, PauseState& pause) {
    idle.bindTargets(&diagnostic, &calibration);
    diagnostic.bindTargets(&idle, &calibration);
    calibration.bindTargets(&idle, &runningNormal);
    runningNormal.bindTargets(&pause, &idle, &runningAlert);
    runningAlert.bindTargets(&pause, &idle, &runningNormal);
    pause.bindTargets(&runningNormal, &idle);
}

void test_initial_state_is_idle() {
    IdleState idle;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    RunningAlertState runningAlert;
    PauseState pause;
    bindAllStates(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    MockFeedback ui;
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause, ui);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
    TEST_ASSERT_EQUAL(FeedbackColor::ORANGE_BREATH, ui.getLastColor());
}

void test_calibration_completes_to_running_normal() {
    IdleState idle;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    RunningAlertState runningAlert;
    PauseState pause;
    bindAllStates(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    MockFeedback ui;
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause, ui);

    fsm.requestTransition(&calibration);
    fsm.update(ui, false, false, 0.0f);

    fsm.requestTransition(&runningNormal);
    fsm.update(ui, false, false, 0.0f);

    TEST_ASSERT_EQUAL(SystemState::RUNNING_NORMAL, fsm.getCurrentState());
    TEST_ASSERT_EQUAL(FeedbackColor::GREEN_FIXED, ui.getLastColor());
    TEST_ASSERT_EQUAL(1500u, ui.getLastFrequencyHz());
}

void test_running_normal_long_press_returns_to_idle() {
    IdleState idle;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    RunningAlertState runningAlert;
    PauseState pause;
    bindAllStates(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    MockFeedback ui;
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause, ui);

    fsm.requestTransition(&runningNormal);
    fsm.update(ui, false, false, 0.0f);
    fsm.update(ui, false, true, 0.0f);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
}
