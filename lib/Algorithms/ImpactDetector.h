#pragma once

// Détection de pic d'impact sur l'axe vertical (fenêtre glissante).
class ImpactDetector {
private:
    float previousVal_;
    float thresholdG_;
    bool isInsideImpact_;
    float currentPeak_;

public:
    explicit ImpactDetector(float thresholdG = 2.0f);
    bool processSample(float currentSample, float& outPeak);
};
