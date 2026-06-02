#include <unity.h>

void test_compute_asymmetry_is_const_pure_math();
void test_calibration_accumulates_steps();
void test_initial_state_is_idle();
void test_calibration_completes_to_running_normal();
void test_running_normal_long_press_returns_to_idle();
void test_serialize_deserialize_round_trip();
void test_deserialize_rejects_short_buffer();

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_compute_asymmetry_is_const_pure_math);
    RUN_TEST(test_calibration_accumulates_steps);
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_calibration_completes_to_running_normal);
    RUN_TEST(test_running_normal_long_press_returns_to_idle);
    RUN_TEST(test_serialize_deserialize_round_trip);
    RUN_TEST(test_deserialize_rejects_short_buffer);
    return UNITY_END();
}
