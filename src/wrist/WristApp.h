#pragma once

#include "../../lib/HAL/Board.h"
#include "../../lib/HAL/Feedback.h"
#include "../../lib/network/NetworkManager.h"
#include "../../lib/core/StateMachine.h"
#include "../../lib/core/WristStatesImpl.h"
#include "../../lib/algorithms/GaitAnalyzer.h"
#include "../../include/Protocol.h"
#include "../../include/Types.h"
#include <cstdint>

// Application principale du boîtier poignet (hub maître).
class WristApp {
private:
    Board& board_;
    Feedback& feedback_;
    NetworkManager& network_;

    ReposState reposState_;
    DiagnosticState diagnosticState_;
    CalibrationState calibrationState_;
    CourseNormalState courseNormalState_;
    CourseAlerteState courseAlerteState_;
    PauseState pauseState_;

    StateMachine fsm_;
    GaitAnalyzer analyzer_;
    float lastLeftImpact_;
    float lastRightImpact_;
    uint32_t lastLeftImpactTime_;
    uint32_t lastRightImpactTime_;
    uint32_t lastLeftSeqNum_;
    uint32_t lastRightSeqNum_;
    float currentAsymmetry_;
    uint32_t lastButtonTime_;
    bool debouncedPressed_;
    bool lastRawPressed_;
    uint32_t lastDebounceChangeMs_;
    bool longPressEmitted_;
    SystemState previousFsmState_;
    uint32_t ledRestoreAt_;
    FeedbackColor ledBasePattern_;
    uint32_t lastCalibrationActivityMs_;

    void bindStateTargets();
    void onStateEntered(SystemState entered, SystemState previous);
    void pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs);
    void restoreLedIfNeeded();
    void processCalibrationImpact(float peak, uint8_t side);
    void processDiagnosticImpact();
    void handleCalibrationTimeout();
    bool isImpactValid(uint32_t impactTime) const;
    bool isAbsoluteThresholdMet(float left, float right) const;
    bool areImpactsPairedForStride() const;
    void enterHardwareFaultLoop();
    void pollButton(bool& btnShort, bool& btnLong);

public:
    WristApp(Board& board, Feedback& feedback, NetworkManager& network);
    void setup();
    void loop();
    void handleIncomingImpact(const ImpactPayload& incoming);
};
