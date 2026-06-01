#include <unity.h>
#include <M5Unified.h>
#include "../../../lib/HAL/M5StickCPlus2/M5Board.h"
#include "../../../lib/HAL/M5StickCPlus2/M5Imu.h"
#include "../../../lib/HAL/M5StickCPlus2/M5Feedback.h"

M5Board board;
M5Imu imu;
M5Feedback feedback;

void setUp(void) {
    board.init();
    imu.init();
}

void tearDown(void) {}

void test_board_battery_level(void) {
    uint8_t level = board.getBatteryLevel();
    TEST_ASSERT_GREATER_OR_EQUAL(0, level);
    TEST_ASSERT_LESS_OR_EQUAL(100, level);
}

void test_imu_acceleration_reading(void) {
    imu.update();
    float accelZ = imu.getAccelerationZ();
    float magnitude = imu.getAccelerationMagnitude();

    TEST_ASSERT_NOT_NAN(accelZ);
    TEST_ASSERT_NOT_NAN(magnitude);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_board_battery_level);
    RUN_TEST(test_imu_acceleration_reading);
    return UNITY_END();
}
