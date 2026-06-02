#include <unity.h>
#include "../../../lib/Algorithms/GaitAnalyzer.h"
#include "../../../include/AppConfig.h"
#include "../../../include/Types.h"

void test_compute_asymmetry_is_const_pure_math()
{
    GaitAnalyzer analyzer;
    const float asymmetry = analyzer.computeAsymmetry(10.0f, 8.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, asymmetry);
}

void test_calibration_accumulates_steps()
{
    GaitAnalyzer analyzer;
    TEST_ASSERT_FALSE(analyzer.addCalibrationStep(5.0f, FootSide::LEFT));
    TEST_ASSERT_EQUAL(1, analyzer.getCalibrationStepCount());
}

void test_asymmetry_below_min_force_returns_zero()
{
    GaitAnalyzer analyzer;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, analyzer.computeAsymmetry(MIN_IMPACT_FORCE_G - 0.1f, 10.0f));
}

void test_asymmetry_equal_forces_is_zero()
{
    GaitAnalyzer analyzer;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, analyzer.computeAsymmetry(8.0f, 8.0f));
}

void test_calibration_30_steps_sets_correct_baselines()
{
    GaitAnalyzer analyzer;
    for (uint8_t i = 0; i < CALIBRATION_REQUIRED_STEPS; ++i)
    {
        const FootSide side = (i % 2 == 0) ? FootSide::LEFT : FootSide::RIGHT;
        const bool done = analyzer.addCalibrationStep(6.0f, side);
        if (i < (CALIBRATION_REQUIRED_STEPS - 1))
        {
            TEST_ASSERT_FALSE(done);
        }
        else
        {
            TEST_ASSERT_TRUE(done);
        }
    }
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 6.0f, analyzer.getLeftBaseline());
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 6.0f, analyzer.getRightBaseline());
}

void test_reset_clears_all_accumulators()
{
    GaitAnalyzer analyzer;
    analyzer.addCalibrationStep(5.0f, FootSide::LEFT);
    analyzer.addCalibrationStep(7.0f, FootSide::RIGHT);
    analyzer.resetCalibration();
    TEST_ASSERT_EQUAL(0, analyzer.getCalibrationStepCount());
    TEST_ASSERT_EQUAL(0, analyzer.getLeftStepCount());
    TEST_ASSERT_EQUAL(0, analyzer.getRightStepCount());
}

void test_calibration_guard_after_completion()
{
    GaitAnalyzer analyzer;
    for (uint8_t i = 0; i < CALIBRATION_REQUIRED_STEPS_PER_SIDE; ++i)
    {
        analyzer.addCalibrationStep(5.0f, FootSide::LEFT);
        analyzer.addCalibrationStep(5.0f, FootSide::RIGHT);
    }
    TEST_ASSERT_TRUE(analyzer.isCalibrationComplete());
    const uint8_t before = analyzer.getCalibrationStepCount();
    TEST_ASSERT_TRUE(analyzer.addCalibrationStep(9.0f, FootSide::RIGHT));
    TEST_ASSERT_EQUAL(before, analyzer.getCalibrationStepCount());
}