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
private:
    uint8_t leftStepCount_ = 0, rightStepCount_ = 0;
    float leftAccumulator_ = 0.0f, rightAccumulator_ = 0.0f;
    float minForceThreshold_ = 3.0f;

    // ─── NOUVEAU : HISTORIQUE POUR MOYENNE MOBILE EN COURSE ───
    static const uint8_t BUFFER_SIZE = 8; // 8 pas par jambe = 16 pas au total
    float leftBuffer_[BUFFER_SIZE] = {0.0f};
    float rightBuffer_[BUFFER_SIZE] = {0.0f};
    uint8_t leftBufferIdx_ = 0;
    uint8_t rightBufferIdx_ = 0;
    uint8_t leftBufferCount_ = 0;
    uint8_t rightBufferCount_ = 0;

public:
    void reset();
    bool addCalibrationStep(float force, bool isLeft);

    // ─── NOUVELLES MÉTHODES DE COURSE ───
    void addRunningStep(float force, bool isLeft);
    float getLeftAverage() const;
    float getRightAverage() const;

    float computeAsymmetry(float leftPeak, float rightPeak) const;
    uint8_t getTotalSteps() const { return leftStepCount_ + rightStepCount_; }
    void setMinForceThreshold(float threshold) { minForceThreshold_ = threshold; }
    float getMinForceThreshold() const { return minForceThreshold_; }
};