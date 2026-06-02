#include "GaitAlgorithms.h"
#include <algorithm>
#include <cmath>

std::optional<float> ImpactDetector::processSample(float currentSample, uint32_t nowMs)
{
    if (!isInsideImpact_ && (nowMs - lastImpactEndMs_) < 250)
        return std::nullopt; // Cooldown 250ms

    if (!isInsideImpact_)
    {
        if (currentSample > thresholdG_)
        {
            isInsideImpact = true;
            currentPeak_ = currentSample;
        }
    }
    else
    {
        currentPeak_ = std::max(currentPeak_, currentSample);
        if (currentSample <= thresholdG_)
        {
            isInsideImpact_ = false;
            lastImpactEndMs_ = nowMs;
            return currentPeak_;
        }
    }
    return std::nullopt;
}

void GaitAnalyzer::reset()
{
    leftStepCount_ = rightStepCount_ = 0;
    leftAccumulator_ = rightAccumulator_ = 0.0f;
}

bool GaitAnalyzer::addCalibrationStep(float force, bool isLeft)
{
    if (isLeft && leftStepCount_ < 15)
    {
        leftAccumulator_ += force;
        leftStepCount_++;
    }
    else if (!isLeft && rightStepCount_ < 15)
    {
        rightAccumulator_ += force;
        rightStepCount_++;
    }
    return (leftStepCount_ >= 15 && rightStepCount_ >= 15); // Terminé à 30 pas (15 de chaque côté)
}

float GaitAnalyzer::computeAsymmetry(float left, float right) const
{
    if (left < 3.0f || right < 3.0f)
        return 0.0f; // Force min
    return (std::abs(left - right) / std::max(left, right)) * 100.0f;
}