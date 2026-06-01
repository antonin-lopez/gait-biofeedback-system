#ifndef IMPACTDETECTOR_H
#define IMPACTDETECTOR_H

class ImpactDetector {
private:
    float _previousVal;
    float _thresholdG;
    bool _isInsideImpact;
    float _currentPeak;

public:
    ImpactDetector(float thresholdG = 2.0f);
    bool processSample(float currentSample, float& outPeak);
};

#endif // IMPACTDETECTOR_H
