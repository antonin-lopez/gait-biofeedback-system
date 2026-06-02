#pragma once

#include "../Feedback.h"
#include <string_view>

class M5Feedback : public Feedback
{
public:
    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void showStatusLine(std::string_view statusLine) override; // Corrigé
    void showAsymmetryPercent(float asymmetryPercent) override;
};