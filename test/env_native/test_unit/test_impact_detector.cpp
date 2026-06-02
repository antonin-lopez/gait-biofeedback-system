#include <unity.h>
#include "../../../lib/Algorithms/ImpactDetector.h"
#include "../../../include/AppConfig.h"

void test_below_threshold_returns_no_impact() {
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);
    float peak = 0.0f;
    TEST_ASSERT_FALSE(detector.processSample(IMPACT_DETECTION_THRESHOLD_G - 0.1f, 0, peak));
}

void test_peak_tracked_across_multi_sample_impact() {
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);
    float peak = 0.0f;

    TEST_ASSERT_FALSE(detector.processSample(2.5f, IMPACT_COOLDOWN_MS, peak));
    TEST_ASSERT_FALSE(detector.processSample(5.2f, IMPACT_COOLDOWN_MS + 10, peak));
    TEST_ASSERT_TRUE(detector.processSample(1.9f, IMPACT_COOLDOWN_MS + 20, peak));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 5.2f, peak);
}

void test_cooldown_prevents_immediate_retrigger() {
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);
    float peak = 0.0f;

    TEST_ASSERT_FALSE(detector.processSample(3.0f, IMPACT_COOLDOWN_MS, peak));
    TEST_ASSERT_TRUE(detector.processSample(1.0f, IMPACT_COOLDOWN_MS + 10, peak));
    TEST_ASSERT_FALSE(
        detector.processSample(3.0f, IMPACT_COOLDOWN_MS + 10 + IMPACT_COOLDOWN_MS - 1, peak));
}

void test_impact_ends_when_signal_falls_below_threshold() {
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);
    float peak = 0.0f;

    TEST_ASSERT_FALSE(detector.processSample(2.2f, IMPACT_COOLDOWN_MS, peak));
    TEST_ASSERT_TRUE(detector.processSample(2.0f, IMPACT_COOLDOWN_MS + 10, peak));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.2f, peak);
}
