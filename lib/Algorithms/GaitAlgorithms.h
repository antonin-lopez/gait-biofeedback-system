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
    void setThreshold(float newThreshold) { thresholdG_ = newThreshold; }
    float getThreshold() const { return thresholdG_; }
};

class GaitAnalyzer
{
public:
    // Remplacement de la valeur magique par une constante propre
    static constexpr uint8_t CALIBRATION_STEPS_PER_SIDE = 16;

private:
    uint8_t leftStepCount_ = 0, rightStepCount_ = 0;
    float leftAccumulator_ = 0.0f, rightAccumulator_ = 0.0f;

    static const uint8_t BUFFER_SIZE = 8;
    float leftBuffer_[BUFFER_SIZE] = {0.0f};
    float rightBuffer_[BUFFER_SIZE] = {0.0f};
    uint8_t leftBufferIdx_ = 0;
    uint8_t rightBufferIdx_ = 0;
    uint8_t leftBufferCount_ = 0;
    uint8_t rightBufferCount_ = 0;

    float minForceThreshold_;
    float personalizedAsymmetryThreshold_ = 10.0f;

public:
    void reset();
    bool addCalibrationStep(float force, bool isLeft);

    void addRunningStep(float force, bool isLeft);
    float getLeftAverage() const;
    float getRightAverage() const;

    float computeAsymmetry(float leftPeak, float rightPeak) const;
    uint8_t getTotalSteps() const { return leftStepCount_ + rightStepCount_; }

    GaitAnalyzer(float minForceThreshold) : minForceThreshold_(minForceThreshold) {}

    void setMinForceThreshold(float threshold) { minForceThreshold_ = threshold; }
    float getMinForceThreshold() const { return minForceThreshold_; }
    float getPersonalizedAsymmetryThreshold() const { return personalizedAsymmetryThreshold_; }
};