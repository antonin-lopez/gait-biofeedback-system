#pragma once

#include <cstdint>
#include <optional>

class ImpactDetector
{
private:
    float thresholdG_;
    bool isInsideImpact_;
    float currentPeak_;
    uint32_t lastImpactEndMs_;

public:
    explicit ImpactDetector(float thresholdG = 2.0f);
    std::optional<float> processSample(float currentSample, uint32_t nowMs);
};