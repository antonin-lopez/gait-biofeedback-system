#pragma once

#include "../Feedback.h"
#include <string_view>

class MockFeedback : public Feedback
{
private:
    FeedbackColor lastColor_ = FeedbackColor::ORANGE_BREATH;
    uint32_t lastFrequencyHz_ = 0;
    uint32_t lastDurationMs_ = 0;

public:
    MockFeedback() = default;

    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void showStatusLine(std::string_view statusLine) override; // Corrigé
    void showAsymmetryPercent(float asymmetryPercent) override;

    FeedbackColor getLastColor() const;
    uint32_t getLastFrequencyHz() const;
    uint32_t getLastDurationMs() const;
};