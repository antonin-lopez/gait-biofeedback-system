#ifndef GAITANALYZER_H
#define GAITANALYZER_H

#include <cstdint>
#include "../../include/Types.h"

class GaitAnalyzer {
private:
    float _leftBaseline;
    float _rightBaseline;
    uint8_t _calibStepCount;
    float _leftAccumulator;
    float _rightAccumulator;

public:
    GaitAnalyzer();
    void resetCalibration();
    bool addCalibrationStep(float peakForce, FootSide side);
    void setBaselines(float left, float right);
    float computeAsymmetry(float leftPeak, float rightPeak);
};

#endif // GAITANALYZER_H
