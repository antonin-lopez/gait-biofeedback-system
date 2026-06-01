#include "ImpactDetector.h"
#include <algorithm>

ImpactDetector::ImpactDetector(float thresholdG)
    : _previousVal(0.0f), _thresholdG(thresholdG), _isInsideImpact(false), _currentPeak(0.0f) {}

bool ImpactDetector::processSample(float currentSample, float& outPeak) {
    bool impactEnded = false;

    if (!_isInsideImpact) {
        if (currentSample > _thresholdG) {
            _isInsideImpact = true;
            _currentPeak = currentSample;
        }
    } else {
        _currentPeak = std::max(_currentPeak, currentSample);

        if (currentSample <= _thresholdG) {
            _isInsideImpact = false;
            outPeak = _currentPeak;
            _currentPeak = 0.0f;
            impactEnded = true;
        }
    }

    _previousVal = currentSample;
    return impactEnded;
}
