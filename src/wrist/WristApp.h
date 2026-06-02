#pragma once
#include "Board.h"
#include "Feedback.h"
#include "NetworkManager.h"
#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "GaitAnalyzer.h"
#include <freertos/FreeRTOS.h>

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
    
    float lastLeftImpact_ = 0.0f;
    float lastRightImpact_ = 0.0f;
    uint32_t lastLeftImpactTime_ = 0;
    uint32_t lastRightImpactTime_ = 0;
    uint32_t lastLeftSeqNum_ = UINT32_MAX;
    uint32_t lastRightSeqNum_ = UINT32_MAX;
    float currentAsymmetry_ = 0.0f;
    
    SystemState previousFsmState_ = SystemState::IDLE;
    float lastDisplayedAsymmetry_ = -1.0f;
    uint32_t lastDisplayUpdateMs_ = 0;
    uint32_t ledRestoreAt_ = 0;
    FeedbackColor ledBasePattern_ = FeedbackColor::ORANGE_BREATH;
    uint32_t lastCalibrationActivityMs_ = 0;
    uint32_t lastLeftHeartbeatMs_ = 0;
    uint32_t lastRightHeartbeatMs_ = 0;
    bool heartbeatBlinkOn_ = false;
    uint32_t lastHeartbeatBlinkMs_ = 0;
    TickType_t xLastWakeTime_;

    void registerStatesInFsm();
    void onStateEntered(SystemState entered, SystemState previous);
    void updateDisplayForState(SystemState state, bool stateChanged);
    void pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs);
    void restoreLedIfNeeded();
    void processCalibrationImpact(float peak, FootSide side);
    void processDiagnosticImpact();
    void handleCalibrationTimeout();
    bool isImpactValid(uint32_t impactTime) const;
    bool isHeartbeatAlive(uint32_t heartbeatTime) const;
    bool isAbsoluteThresholdMet(float left, float right) const;
    bool areImpactsPairedForStride() const;
    bool areBothAnklesConnected() const;
    void handleHeartbeatTimeoutUi();
    void handleHardwareInitFailure();

public:
    WristApp(Board& board, Feedback& feedback, NetworkManager& network);
    void setup();
    void loop();
    void handleIncomingImpact(const ImpactPayload& incoming);
};