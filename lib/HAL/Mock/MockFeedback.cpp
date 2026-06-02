#include "MockFeedback.h"

void MockFeedback::setLedPattern(FeedbackColor color)
{
    lastColor_ = color;
}

void MockFeedback::triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs)
{
    lastFrequencyHz_ = frequencyHz;
    lastDurationMs_ = durationMs;
}

void MockFeedback::showStatusLine(std::string_view statusLine)
{
    (void)statusLine;
}

void MockFeedback::showAsymmetryPercent(float asymmetryPercent)
{
    (void)asymmetryPercent;
}

FeedbackColor MockFeedback::getLastColor() const
{
    return lastColor_;
}

uint32_t MockFeedback::getLastFrequencyHz() const
{
    return lastFrequencyHz_;
}

uint32_t MockFeedback::getLastDurationMs() const
{
    return lastDurationMs_;
}