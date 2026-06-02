#pragma once

#include "../../include/Types.h"
#include <cstdint>
#include <string_view>

class Feedback
{
public:
    virtual ~Feedback() = default;
    virtual void setLedPattern(FeedbackColor color) = 0;
    virtual void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) = 0;
    virtual void showStatusLine(std::string_view statusLine) = 0; // Sécurisé
    virtual void showAsymmetryPercent(float asymmetryPercent) = 0;
};