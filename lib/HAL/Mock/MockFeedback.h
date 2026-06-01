#ifndef MOCKFEEDBACK_H
#define MOCKFEEDBACK_H

#include "../IFeedback.h"

class MockFeedback : public IFeedback {
public:
    FeedbackColor lastColorMock = FeedbackColor::ORANGE_BREATH;
    uint32_t lastFrequencyMock = 0;
    uint32_t lastDurationMock = 0;

    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void updateDisplay(SystemState state, float currentAsymmetry) override;
};

#endif // MOCKFEEDBACK_H
