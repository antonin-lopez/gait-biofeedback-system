#include "MockFeedback.h"

MockFeedback::MockFeedback()
    : lastColor_(FeedbackColor::ORANGE_BREATH), lastFrequencyHz_(0), lastDurationMs_(0) {}

void MockFeedback::setLedPattern(FeedbackColor color) {
    lastColor_ = color;
}

void MockFeedback::triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) {
    lastFrequencyHz_ = frequencyHz;
    lastDurationMs_ = durationMs;
}

void MockFeedback::updateDisplay(SystemState state, float currentAsymmetry) {
    (void)state;
    (void)currentAsymmetry;
}

FeedbackColor MockFeedback::getLastColor() const {
    return lastColor_;
}

uint32_t MockFeedback::getLastFrequencyHz() const {
    return lastFrequencyHz_;
}

uint32_t MockFeedback::getLastDurationMs() const {
    return lastDurationMs_;
}
