#pragma once

#include "../../include/Types.h"
#include <cstdint>

// Interface retour utilisateur (LED, buzzer, écran) — agnostique du domaine métier.
class Feedback {
public:
    virtual ~Feedback() = default;
    virtual void setLedPattern(FeedbackColor color) = 0;
    virtual void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) = 0;
    virtual void showStatusLine(const char* statusLine) = 0;
    virtual void showAsymmetryPercent(float asymmetryPercent) = 0;
};
