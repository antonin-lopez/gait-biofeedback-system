#pragma once

#include "Board.h"
#include "Feedback.h"
#include "NetworkManager.h"
#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "GaitAnalyzer.h"
#include "Protocol.h"
#include "Types.h"
#include <cstdint>

// Application principale du boîtier poignet (hub maître).
class WristApp {
private:
    Board& board_;
    Feedback& feedback_;
    NetworkManager& network_;

    IdleState idleState_;
    DiagnosticState diagnosticState_;
    CalibrationState calibrationState_;
    RunningNormalState runningNormalState_;
    RunningAlertState runningAlertState_;
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
    float lastDisplayedAsymmetry_;
    uint32_t lastDisplayUpdateMs_;
    uint32_t ledRestoreAt_;
    FeedbackColor ledBasePattern_;
    uint32_t lastCalibrationActivityMs_;

    void bindStateTargets();
    void onStateEntered(SystemState entered, SystemState previous);
    void updateDisplayForState(SystemState state, bool stateChanged);
    void pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs);
    void restoreLedIfNeeded();
    void processCalibrationImpact(float peak, uint8_t side);
    void processDiagnosticImpact();
    void handleCalibrationTimeout();
    bool isImpactValid(uint32_t impactTime) const;
    bool isAbsoluteThresholdMet(float left, float right) const;
    bool areImpactsPairedForStride() const;
    void handleHardwareInitFailure();
    void pollButton(bool& btnShort, bool& btnLong);

public:
    WristApp(Board& board, Feedback& feedback, NetworkManager& network);
    void setup();
    void loop();
    void handleIncomingImpact(const ImpactPayload& incoming);
};
