#pragma once

#include <cstdint>
#include "../../include/Types.h"
#include "../../include/AppConfig.h"

// Analyse d'asymétrie de foulée et calibration des baselines.
class GaitAnalyzer
{
private:
    float leftBaseline_ = 0.0f;
    float rightBaseline_ = 0.0f;
    uint8_t calibStepCount_ = 0;
    uint8_t leftStepCount_ = 0;
    uint8_t rightStepCount_ = 0;
    float leftAccumulator_ = 0.0f;
    float rightAccumulator_ = 0.0f;

public:
    GaitAnalyzer() = default;
    void resetCalibration();
    bool addCalibrationStep(float peakForce, FootSide side);
    void setBaselines(float left, float right);
    float computeAsymmetry(float leftPeak, float rightPeak) const;

    float getLeftBaseline() const { return leftBaseline_; }
    float getRightBaseline() const { return rightBaseline_; }
    uint8_t getLeftStepCount() const { return leftStepCount_; }
    uint8_t getRightStepCount() const { return rightStepCount_; }
    uint8_t getCalibrationStepCount() const { return calibStepCount_; }
    bool isCalibrationComplete() const
    {
        return (leftStepCount_ >= CALIBRATION_REQUIRED_STEPS_PER_SIDE &&
                rightStepCount_ >= CALIBRATION_REQUIRED_STEPS_PER_SIDE);
    }
};