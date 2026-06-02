#include <unity.h>
#include "test_gait_analyzer.h"
#include "test_state_machine.h"
#include "test_protocol_codec.h"
#include "test_impact_detector.h"

void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_compute_asymmetry_is_const_pure_math);
    RUN_TEST(test_calibration_accumulates_steps);
    RUN_TEST(test_asymmetry_below_min_force_returns_zero);
    RUN_TEST(test_asymmetry_equal_forces_is_zero);
    RUN_TEST(test_calibration_30_steps_sets_correct_baselines);
    RUN_TEST(test_reset_clears_all_accumulators);
    RUN_TEST(test_calibration_guard_after_completion);
    RUN_TEST(test_initial_state_is_idle);
    RUN_TEST(test_calibration_completes_to_running_normal);
    RUN_TEST(test_running_normal_long_press_returns_to_idle);
    RUN_TEST(test_serialize_deserialize_round_trip);
    RUN_TEST(test_deserialize_rejects_short_buffer);
    RUN_TEST(test_deserialize_rejects_unknown_protocol_version);
    RUN_TEST(test_serialize_deserialize_heartbeat_round_trip);
    RUN_TEST(test_below_threshold_returns_no_impact);
    RUN_TEST(test_peak_tracked_across_multi_sample_impact);
    RUN_TEST(test_cooldown_prevents_immediate_retrigger);
    RUN_TEST(test_impact_ends_when_signal_falls_below_threshold);
    return UNITY_END();
}
