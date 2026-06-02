#include <unity.h>
#include "../../../lib/Algorithms/ImpactDetector.h"
#include "../../../include/AppConfig.h"

void test_below_threshold_returns_no_impact()
{
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);
    auto res = detector.processSample(IMPACT_DETECTION_THRESHOLD_G - 0.1f, 0);
    TEST_ASSERT_FALSE(res.has_value());
}

void test_peak_tracked_across_multi_sample_impact()
{
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);

    TEST_ASSERT_FALSE(detector.processSample(2.5f, IMPACT_COOLDOWN_MS).has_value());
    TEST_ASSERT_FALSE(detector.processSample(5.2f, IMPACT_COOLDOWN_MS + 10).has_value());

    auto res = detector.processSample(1.9f, IMPACT_COOLDOWN_MS + 20);
    TEST_ASSERT_TRUE(res.has_value());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 5.2f, res.value());
}

void test_cooldown_prevents_immediate_retrigger()
{
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);

    TEST_ASSERT_FALSE(detector.processSample(3.0f, IMPACT_COOLDOWN_MS).has_value());
    TEST_ASSERT_TRUE(detector.processSample(1.0f, IMPACT_COOLDOWN_MS + 10).has_value());
    TEST_ASSERT_FALSE(
        detector.processSample(3.0f, IMPACT_COOLDOWN_MS + 10 + IMPACT_COOLDOWN_MS - 1).has_value());
}

void test_impact_ends_when_signal_falls_below_threshold()
{
    ImpactDetector detector(IMPACT_DETECTION_THRESHOLD_G);

    TEST_ASSERT_FALSE(detector.processSample(2.2f, IMPACT_COOLDOWN_MS).has_value());
    auto res = detector.processSample(2.0f, IMPACT_COOLDOWN_MS + 10);
    TEST_ASSERT_TRUE(res.has_value());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.2f, res.value());
}