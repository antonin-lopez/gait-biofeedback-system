#include <unity.h>
#include "../../../lib/Algorithms/GaitAnalyzer.h"
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

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_perfect_symmetry);
    RUN_TEST(test_asymmetry_calculation);
    RUN_TEST(test_calibration_baseline_completion);
    return UNITY_END();
}
