#include "GaitAnalyzer.h"
#include "../../include/AppConfig.h"
#include <algorithm>

GaitAnalyzer::GaitAnalyzer()
    : _leftBaseline(0.0f), _rightBaseline(0.0f), _calibStepCount(0),
      _leftAccumulator(0.0f), _rightAccumulator(0.0f) {}

void GaitAnalyzer::resetCalibration() {
    _calibStepCount = 0;
    _leftAccumulator = 0.0f;
    _rightAccumulator = 0.0f;
}

bool GaitAnalyzer::addCalibrationStep(float peakForce, FootSide side) {
    if (side == FootSide::LEFT) {
        _leftAccumulator += peakForce;
    } else {
        _rightAccumulator += peakForce;
    }

    _calibStepCount++;

    if (_calibStepCount >= CALIBRATION_REQUIRED_STEPS) {
        _leftBaseline = _leftAccumulator / (CALIBRATION_REQUIRED_STEPS / 2.0f);
        _rightBaseline = _rightAccumulator / (CALIBRATION_REQUIRED_STEPS / 2.0f);
        return true;
    }

    return false;
}

void GaitAnalyzer::setBaselines(float left, float right) {
    _leftBaseline = left;
    _rightBaseline = right;
}

float GaitAnalyzer::computeAsymmetry(float leftPeak, float rightPeak) {
    if (leftPeak == 0.0f || rightPeak == 0.0f) {
        return 0.0f;
    }

    float diff = std::abs(leftPeak - rightPeak);
    float maxVal = std::max(leftPeak, rightPeak);
    return (diff / maxVal) * 100.0f;
}
