#include <unity.h>
#include "../../../lib/algorithms/GaitAnalyzer.h"
#include "../../../include/Types.h"

void setUp(void) {}
void tearDown(void) {}

void test_perfect_symmetry(void) {
    GaitAnalyzer analyzer;
    float asymmetry = analyzer.computeAsymmetry(10.0f, 10.0f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, asymmetry);
}

void test_asymmetry_calculation(void) {
    GaitAnalyzer analyzer;
    float asymmetry = analyzer.computeAsymmetry(10.0f, 8.0f);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, asymmetry);
}

void test_asymmetry_near_zero_peaks(void) {
    GaitAnalyzer analyzer;
    TEST_ASSERT_EQUAL_FLOAT(0.0f, analyzer.computeAsymmetry(0.0f, 10.0f));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, analyzer.computeAsymmetry(10.0f, 0.0005f));
    TEST_ASSERT_EQUAL_FLOAT(0.0f, analyzer.computeAsymmetry(0.0005f, 10.0f));
}

void test_calibration_baseline_completion(void) {
    GaitAnalyzer analyzer;
    bool completed = false;

    for (int i = 0; i < 30; i++) {
        completed = analyzer.addCalibrationStep(10.0f, FootSide::LEFT);
        if (i < 29) {
            TEST_ASSERT_FALSE(completed);
        }
    }

    TEST_ASSERT_TRUE(completed);
}

void test_calibration_uneven_step_distribution(void) {
    GaitAnalyzer analyzer;

    for (int i = 0; i < 16; i++) {
        analyzer.addCalibrationStep(12.0f, FootSide::LEFT);
    }
    for (int i = 0; i < 14; i++) {
        analyzer.addCalibrationStep(8.0f, FootSide::RIGHT);
    }

    TEST_ASSERT_EQUAL_UINT8(16, analyzer.getLeftStepCount());
    TEST_ASSERT_EQUAL_UINT8(14, analyzer.getRightStepCount());
    TEST_ASSERT_EQUAL_FLOAT(12.0f, analyzer.getLeftBaseline());
    TEST_ASSERT_EQUAL_FLOAT(8.0f, analyzer.getRightBaseline());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_perfect_symmetry);
    RUN_TEST(test_asymmetry_calculation);
    RUN_TEST(test_asymmetry_near_zero_peaks);
    RUN_TEST(test_calibration_baseline_completion);
    RUN_TEST(test_calibration_uneven_step_distribution);
    return UNITY_END();
}
