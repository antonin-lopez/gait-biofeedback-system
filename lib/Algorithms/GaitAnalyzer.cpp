#include "GaitAnalyzer.h"
#include "../../include/AppConfig.h"
#include <algorithm>
#include <cmath>

void GaitAnalyzer::resetCalibration()
{
    calibStepCount_ = 0;
    leftStepCount_ = 0;
    rightStepCount_ = 0;
    leftAccumulator_ = 0.0f;
    rightAccumulator_ = 0.0f;
}

bool GaitAnalyzer::addCalibrationStep(float peakForce, FootSide side)
{
    if (isCalibrationComplete())
    {
        return true;
    }

    if (side == FootSide::LEFT && leftStepCount_ < CALIBRATION_REQUIRED_STEPS_PER_SIDE)
    {
        leftAccumulator_ += peakForce;
        leftStepCount_++;
        calibStepCount_++;
    }
    else if (side == FootSide::RIGHT && rightStepCount_ < CALIBRATION_REQUIRED_STEPS_PER_SIDE)
    {
        rightAccumulator_ += peakForce;
        rightStepCount_++;
        calibStepCount_++;
    }

    if (isCalibrationComplete())
    {
        leftBaseline_ = leftAccumulator_ / static_cast<float>(leftStepCount_);
        rightBaseline_ = rightAccumulator_ / static_cast<float>(rightStepCount_);
        return true;
    }

    return false;
}

void GaitAnalyzer::setBaselines(float left, float right)
{
    leftBaseline_ = left;
    rightBaseline_ = right;
}

float GaitAnalyzer::computeAsymmetry(float leftPeak, float rightPeak) const
{
    if (leftPeak < MIN_IMPACT_FORCE_G || rightPeak < MIN_IMPACT_FORCE_G)
    {
        return 0.0f;
    }

    const float diff = std::abs(leftPeak - rightPeak);
    const float maxVal = std::max(leftPeak, rightPeak);
    return (diff / maxVal) * 100.0f;
}