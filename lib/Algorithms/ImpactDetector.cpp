#include "ImpactDetector.h"
#include <algorithm>

ImpactDetector::ImpactDetector(float thresholdG)
    : previousVal_(0.0f), thresholdG_(thresholdG), isInsideImpact_(false), currentPeak_(0.0f) {}

bool ImpactDetector::processSample(float currentSample, float& outPeak) {
    bool impactEnded = false;

    if (!isInsideImpact_) {
        if (currentSample > thresholdG_) {
            isInsideImpact_ = true;
            currentPeak_ = currentSample;
        }
    } else {
        currentPeak_ = std::max(currentPeak_, currentSample);

        if (currentSample <= thresholdG_) {
            isInsideImpact_ = false;
            outPeak = currentPeak_;
            currentPeak_ = 0.0f;
            impactEnded = true;
        }
    }

    previousVal_ = currentSample;
    return impactEnded;
}
