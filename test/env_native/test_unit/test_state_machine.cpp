#include <unity.h>
#include "../../../lib/Core/StateMachine.h"
#include "../../../lib/Core/WristStatesImpl.h"
#include "../../../lib/HAL/Mock/MockFeedback.h"
#include "../../../include/Types.h"

void test_initial_state_is_idle()
{
    IdleState idle;
    MockFeedback ui;
    StateMachine fsm;

    fsm.registerState(&idle);
    fsm.setInitialState(SystemState::IDLE, ui);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
    TEST_ASSERT_EQUAL(FeedbackColor::ORANGE_BREATH, ui.getLastColor());
}

void test_calibration_completes_to_running_normal()
{
    IdleState idle;
    CalibrationState calibration;
    RunningNormalState runningNormal;
    MockFeedback ui;
    StateMachine fsm;

    fsm.registerState(&idle);
    fsm.registerState(&calibration);
    fsm.registerState(&runningNormal);

    fsm.setInitialState(SystemState::IDLE, ui);

    fsm.requestTransition(SystemState::CALIBRATION);
    SystemContext ctx{false, false, 0.0f};
    fsm.update(ui, ctx);
    TEST_ASSERT_EQUAL(SystemState::CALIBRATION, fsm.getCurrentState());

    fsm.requestTransition(SystemState::RUNNING_NORMAL);
    fsm.update(ui, ctx);

    TEST_ASSERT_EQUAL(SystemState::RUNNING_NORMAL, fsm.getCurrentState());
    TEST_ASSERT_EQUAL(FeedbackColor::GREEN_FIXED, ui.getLastColor());
    TEST_ASSERT_EQUAL(1500u, ui.getLastFrequencyHz());
}

void test_running_normal_long_press_returns_to_idle()
{
    IdleState idle;
    RunningNormalState runningNormal;
    MockFeedback ui;
    StateMachine fsm;

    fsm.registerState(&idle);
    fsm.registerState(&runningNormal);

    fsm.setInitialState(SystemState::RUNNING_NORMAL, ui);

    SystemContext ctx{false, true, 0.0f}; // Simulation d'une pression longue
    fsm.update(ui, ctx);

    TEST_ASSERT_EQUAL(SystemState::IDLE, fsm.getCurrentState());
    TEST_ASSERT_EQUAL(FeedbackColor::ORANGE_BREATH, ui.getLastColor());
}