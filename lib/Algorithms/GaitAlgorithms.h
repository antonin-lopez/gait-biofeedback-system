#pragma once
#include <optional>
#include <cstdint>

class ImpactDetector
{
private:
    float thresholdG_;
    bool isInsideImpact_ = false;
    float currentPeak_ = 0.0f;
    uint32_t lastImpactEndMs_ = 0;

public:
    ImpactDetector(float thresholdG) : thresholdG_(thresholdG) {}
    std::optional<float> processSample(float currentSample, uint32_t nowMs);
};

class GaitAnalyzer
{
private:
    uint8_t leftStepCount_ = 0, rightStepCount_ = 0;
    float leftAccumulator_ = 0.0f, rightAccumulator_ = 0.0f;

public:
    void reset();
    bool addCalibrationStep(float force, bool isLeft);
    float computeAsymmetry(float leftPeak, float rightPeak) const;
    uint8_t getTotalSteps() const { return leftStepCount_ + rightStepCount_; }
};