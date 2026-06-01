#include <unity.h>
#include "../../../lib/Core/StateMachine.h"
#include "../../../lib/HAL/Mock/MockFeedback.h"
#include "../../../include/Types.h"

void setUp(void) {}
void tearDown(void) {}

void test_initial_state_is_repos(void) {
    StateMachine fsm;
    TEST_ASSERT_EQUAL(SystemState::REPOS, fsm.getCurrentState());
}

void test_state_transition(void) {
    StateMachine fsm;
    MockFeedback mockUi;

    // Request REPOS -> DIAGNOSTIC
    fsm.requestTransition(SystemState::DIAGNOSTIC);
    fsm.update(&mockUi, false, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::DIAGNOSTIC, fsm.getCurrentState());

    // Request DIAGNOSTIC -> COURSE_NORMAL
    fsm.requestTransition(SystemState::COURSE_NORMAL);
    fsm.update(&mockUi, false, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::COURSE_NORMAL, fsm.getCurrentState());
}

void test_button_short_in_repos(void) {
    StateMachine fsm;
    MockFeedback mockUi;

    TEST_ASSERT_EQUAL(SystemState::REPOS, fsm.getCurrentState());
    fsm.update(&mockUi, true, false, 0.0f);
    TEST_ASSERT_EQUAL(SystemState::DIAGNOSTIC, fsm.getCurrentState());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_repos);
    RUN_TEST(test_state_transition);
    RUN_TEST(test_button_short_in_repos);
    return UNITY_END();
}

