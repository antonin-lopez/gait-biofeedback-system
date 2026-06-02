#include "ImpactDetector.h"
#include "../../include/AppConfig.h"
#include <algorithm>

ImpactDetector::ImpactDetector(float thresholdG)
    : thresholdG_(thresholdG), isInsideImpact_(false), currentPeak_(0.0f), lastImpactEndMs_(0) {}

std::optional<float> ImpactDetector::processSample(float currentSample, uint32_t nowMs)
{
    if (!isInsideImpact_ && (nowMs - lastImpactEndMs_) < IMPACT_COOLDOWN_MS)
    {
        return std::nullopt;
    }

    if (!isInsideImpact_)
    {
        if (currentSample > thresholdG_)
        {
            isInsideImpact_ = true;
            currentPeak_ = currentSample;
        }
    }
    else
    {
        currentPeak_ = std::max(currentPeak_, currentSample);

        if (currentSample <= thresholdG_)
        {
            isInsideImpact_ = false;
            float peak = currentPeak_;
            currentPeak_ = 0.0f;
            lastImpactEndMs_ = nowMs;
            return peak;
        }
    }
    return std::nullopt;
}