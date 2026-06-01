#pragma once

#include "../Feedback.h"

class MockFeedback : public Feedback {
private:
    FeedbackColor lastColor_;
    uint32_t lastFrequencyHz_;
    uint32_t lastDurationMs_;

public:
    MockFeedback();

    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void updateDisplay(SystemState state, float currentAsymmetry) override;

    FeedbackColor getLastColor() const;
    uint32_t getLastFrequencyHz() const;
    uint32_t getLastDurationMs() const;
};
