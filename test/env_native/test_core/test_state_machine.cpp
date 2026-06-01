#include <unity.h>
#include "../../../lib/core/StateMachine.h"
#include "../../../lib/core/WristStatesImpl.h"
#include "../../../lib/HAL/Mock/MockFeedback.h"
#include "../../../include/Types.h"

void setUp() {}
void tearDown() {}

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
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
}

void test_calibration_completes_to_running_normal() {
    IdleState idle;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    RunningAlertState runningAlert;
    PauseState pause;
    bindAllStates(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    MockFeedback ui;

    fsm.requestTransition(&calibration);
    fsm.update(ui, false, false, 0.0f);

    fsm.requestTransition(&runningNormal);
    fsm.update(ui, false, false, 0.0f);

    TEST_ASSERT_EQUAL(SystemState::RUNNING_NORMAL, fsm.getCurrentState());
}

void test_running_normal_long_press_returns_to_idle() {
    IdleState idle;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    RunningAlertState runningAlert;
    PauseState pause;
    bindAllStates(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    StateMachine fsm(idle, diagnostic, calibration, runningNormal, runningAlert, pause);
    MockFeedback ui;

    fsm.requestTransition(&runningNormal);
    fsm.update(ui, false, false, 0.0f);
    fsm.update(ui, false, true, 0.0f);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_calibration_completes_to_running_normal);
    RUN_TEST(test_running_normal_long_press_returns_to_idle);
    return UNITY_END();
}