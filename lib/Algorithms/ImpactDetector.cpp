#include "ImpactDetector.h"
#include "../../include/AppConfig.h"
#include <algorithm>

ImpactDetector::ImpactDetector(float thresholdG)
    : thresholdG_(thresholdG),
      isInsideImpact_(false),
      currentPeak_(0.0f),
      lastImpactEndMs_(0) {}

bool ImpactDetector::processSample(float currentSample, uint32_t nowMs, float& outPeak) {
    bool impactEnded = false;

    if (!isInsideImpact_ && (nowMs - lastImpactEndMs_) < IMPACT_COOLDOWN_MS) {
        return false;
    }

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
            lastImpactEndMs_ = nowMs;
            impactEnded = true;
        }
    }

    return impactEnded;
}
