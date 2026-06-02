#include <unity.h>
#include "../../../lib/algorithms/GaitAnalyzer.h"
#include "../../../include/Types.h"

void test_compute_asymmetry_is_const_pure_math() {
    GaitAnalyzer analyzer;
    const float asymmetry = analyzer.computeAsymmetry(10.0f, 8.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, asymmetry);
}

void test_calibration_accumulates_steps() {
    GaitAnalyzer analyzer;
    TEST_ASSERT_FALSE(analyzer.addCalibrationStep(5.0f, FootSide::LEFT));
    TEST_ASSERT_EQUAL(1, analyzer.getCalibrationStepCount());
}
