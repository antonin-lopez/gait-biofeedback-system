#pragma once

#include <cstdint>

// Détection de pic d'impact sur l'axe vertical (fenêtre glissante).
class ImpactDetector {
private:
    float thresholdG_;
    bool isInsideImpact_;
    float currentPeak_;
    uint32_t lastImpactEndMs_;

public:
    explicit ImpactDetector(float thresholdG = 2.0f);
    bool processSample(float currentSample, uint32_t nowMs, float& outPeak);
};
