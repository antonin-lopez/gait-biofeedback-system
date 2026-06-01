#pragma once

#include "../Feedback.h"

class M5Feedback : public Feedback {
public:
    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void updateDisplay(SystemState state, float currentAsymmetry) override;
};
