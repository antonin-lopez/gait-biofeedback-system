#ifndef IFEEDBACK_H
#define IFEEDBACK_H

#include "../../include/Types.h"

class IFeedback {
public:
    virtual ~IFeedback() = default;
    virtual void setLedPattern(FeedbackColor color) = 0;
    virtual void triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) = 0;
    virtual void updateDisplay(SystemState state, float currentAsymmetry) = 0;
};

#endif // IFEEDBACK_H

