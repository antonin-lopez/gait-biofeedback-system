#include <unity.h>
#include "../../../lib/Core/StateMachine.h"
#include "../../../include/Types.h"

void setUp(void) {}
void tearDown(void) {}

void test_initial_state_is_repos(void) {
    StateMachine fsm;
    TEST_ASSERT_EQUAL(SystemState::REPOS, fsm.getCurrentState());
}

void test_state_transition(void) {
    StateMachine fsm;
    fsm.transitionTo(SystemState::DIAGNOSTIC);
    TEST_ASSERT_EQUAL(SystemState::DIAGNOSTIC, fsm.getCurrentState());

    fsm.transitionTo(SystemState::COURSE_NORMAL);
    TEST_ASSERT_EQUAL(SystemState::COURSE_NORMAL, fsm.getCurrentState());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_repos);
    RUN_TEST(test_state_transition);
    return UNITY_END();
}
