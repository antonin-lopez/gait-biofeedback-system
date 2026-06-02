#include <unity.h>
#include <M5Unified.h>
#include "M5Board.h"
#include "M5Imu.h"
#include "M5Feedback.h"

void test_board_init_and_button_readable()
{
    M5Board board;
    TEST_ASSERT_TRUE(board.init());
    (void)board.consumeShortPress();
    (void)board.consumeLongPress();
}

void test_imu_reads_acceleration()
{
    M5Imu imu;
    TEST_ASSERT_TRUE(imu.init());
    imu.update();
    (void)imu.getAccelerationZ();
}

void test_feedback_led_and_display()
{
    M5Feedback feedback;
    feedback.setLedPattern(FeedbackColor::GREEN_FIXED);
    feedback.showStatusLine("TEST");
    feedback.showAsymmetryPercent(0.0f);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_board_init_and_button_readable);
    RUN_TEST(test_imu_reads_acceleration);
    RUN_TEST(test_feedback_led_and_display);
    return UNITY_END();
}