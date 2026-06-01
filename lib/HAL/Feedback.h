#pragma once

#include "../../include/Types.h"

// Interface retour utilisateur (LED, buzzer, écran).
class Feedback {
public:
    virtual ~Feedback() = default;
    virtual void setLedPattern(FeedbackColor color) = 0;
    virtual void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) = 0;
    virtual void updateDisplay(SystemState state, float currentAsymmetry) = 0;
};
