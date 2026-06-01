#ifndef M5FEEDBACK_H
#define M5FEEDBACK_H

#include "../IFeedback.h"

class M5Feedback : public IFeedback {
public:
    void setLedPattern(FeedbackColor color) override;
    void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) override;
    void updateDisplay(SystemState state, float currentAsymmetry) override;
};

#endif // M5FEEDBACK_H
