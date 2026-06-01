#include "MockFeedback.h"

void MockFeedback::setLedPattern(FeedbackColor color) {
    lastColorMock = color;
}

void MockFeedback::triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) {
    lastFrequencyMock = frequencyHz;
    lastDurationMock = durationMs;
}

void MockFeedback::updateDisplay(SystemState state, float currentAsymmetry) {
}
