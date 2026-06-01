#include <unity.h>
#include "../../../lib/core/StateMachine.h"
#include "../../../lib/core/WristStatesImpl.h"
#include "../../../lib/HAL/Mock/MockFeedback.h"
#include "../../../include/Types.h"

void setUp(void) {}
void tearDown(void) {}

static void bindAllStates(ReposState& repos, DiagnosticState& diagnostic, CalibrationState& calibration,
                          CourseNormalState& courseNormal, CourseAlerteState& courseAlerte, PauseState& pause) {
    repos.bindTargets(&diagnostic, &calibration);
    diagnostic.bindTargets(&repos, &calibration);
    calibration.bindTargets(&repos, &courseNormal);
    courseNormal.bindTargets(&pause, &repos);
    courseAlerte.bindTargets(&pause, &repos);
    pause.bindTargets(&courseNormal, &repos);
}

void test_initial_state_is_repos(void) {
    ReposState repos;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    CourseNormalState courseNormal;
    CourseAlerteState courseAlerte;
    PauseState pause;
    bindAllStates(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);
    StateMachine fsm(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);

    TEST_ASSERT_EQUAL(SystemState::REPOS, fsm.getCurrentState());
}

void test_state_transition(void) {
    ReposState repos;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    CourseNormalState courseNormal;
    CourseAlerteState courseAlerte;
    PauseState pause;
    bindAllStates(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);
    StateMachine fsm(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);
    MockFeedback mockUi;

    fsm.requestTransition(&diagnostic);
    fsm.update(mockUi, false, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::DIAGNOSTIC, fsm.getCurrentState());

    fsm.requestTransition(&calibration);
    fsm.update(mockUi, false, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::CALIBRATION, fsm.getCurrentState());

    fsm.requestTransition(&courseNormal);
    fsm.update(mockUi, false, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::COURSE_NORMAL, fsm.getCurrentState());
}

void test_button_short_in_repos(void) {
    ReposState repos;
    DiagnosticState diagnostic;
    CalibrationState calibration;
    CourseNormalState courseNormal;
    CourseAlerteState courseAlerte;
    PauseState pause;
    bindAllStates(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);
    StateMachine fsm(repos, diagnostic, calibration, courseNormal, courseAlerte, pause);
    MockFeedback mockUi;

    TEST_ASSERT_EQUAL(SystemState::REPOS, fsm.getCurrentState());
    fsm.update(mockUi, true, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::DIAGNOSTIC, fsm.getCurrentState());
}

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_repos);
    RUN_TEST(test_state_transition);
    RUN_TEST(test_button_short_in_repos);
    return UNITY_END();
}
