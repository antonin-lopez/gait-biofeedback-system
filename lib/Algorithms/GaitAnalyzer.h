#pragma once

#include <cstdint>
#include "../../include/Types.h"

// Analyse d'asymétrie de foulée et calibration des baselines.
class GaitAnalyzer {
private:
    float leftBaseline_;
    float rightBaseline_;
    uint8_t calibStepCount_;
    uint8_t leftStepCount_;
    uint8_t rightStepCount_;
    float leftAccumulator_;
    float rightAccumulator_;

public:
    GaitAnalyzer();
    void resetCalibration();
    bool addCalibrationStep(float peakForce, FootSide side);
    void setBaselines(float left, float right);
    float computeAsymmetry(float leftPeak, float rightPeak);

    float getLeftBaseline() const { return leftBaseline_; }
    float getRightBaseline() const { return rightBaseline_; }
    uint8_t getLeftStepCount() const { return leftStepCount_; }
    uint8_t getRightStepCount() const { return rightStepCount_; }
    uint8_t getCalibrationStepCount() const { return calibStepCount_; }
};
