#pragma once

#include "Board.h"
#include "Feedback.h"
#include "NetworkManager.h"
#include "StateMachine.h"
#include "WristStatesImpl.h"
#include "GaitAnalyzer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cstdint>

class WristApp
{
private:
    // Références aux composants matériels (Inversion de dépendance)
    Board &board_;
    Feedback &feedback_;
    NetworkManager &network_;

    // États de la FSM (Allocation statique pour éviter la fragmentation du tas)
    IdleState idleState_;
    DiagnosticState diagnosticState_;
    CalibrationState calibrationState_;
    RunningNormalState runningNormalState_;
    RunningAlertState runningAlertState_;
    PauseState pauseState_;

    StateMachine fsm_;
    GaitAnalyzer analyzer_;

    // Données de foulées et synchronisation des impacts
    float lastLeftImpact_ = 0.0f;
    float lastRightImpact_ = 0.0f;
    uint32_t lastLeftImpactTime_ = 0;
    uint32_t lastRightImpactTime_ = 0;
    uint32_t lastLeftSeqNum_ = UINT32_MAX;
    uint32_t lastRightSeqNum_ = UINT32_MAX;
    float currentAsymmetry_ = 0.0f;

    // Variables de contrôle d'état et d'affichage temporel
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
    TickType_t xLastWakeTime_{0};

    // --- Fonctions privées d'orchestration (Découpage SRP) ---
    uint32_t getCurrentTimeMs() const noexcept { return pdTICKS_TO_MS(xTaskGetTickCount()); }
    void pollNetworkChannels();
    void invalidateStaleData();
    void processGaitAnalysis(bool newImpact);
    void updateSystemStateMachine();
    void maintainUserFeedback(SystemState state, bool stateChanged);

    // --- Sous-fonctions utilitaires et événementielles ---
    void registerStatesInFsm();
    void onStateEntered(SystemState entered, SystemState previous);
    void pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs);
    void restoreLedIfNeeded();
    void processCalibrationImpact(float peak, FootSide side);
    void processDiagnosticImpact();
    void handleCalibrationTimeout();
    void handleHeartbeatTimeoutUi();
    void handleHardwareInitFailure();

    // --- Prédicats de validation constants ---
    bool isImpactValid(uint32_t impactTime) const;
    bool isHeartbeatAlive(uint32_t heartbeatTime) const;
    bool isAbsoluteThresholdMet(float left, float right) const;
    bool areImpactsPairedForStride() const;
    bool areBothAnklesConnected() const;

public:
    WristApp(Board &board, Feedback &feedback, NetworkManager &network);

    // Règle des 5 : Suppression explicite des constructeurs de copie/déplacement
    // pour interdire toute duplication accidentelle de l'objet maître
    WristApp(const WristApp &) = delete;
    WristApp &operator=(const WristApp &) = delete;
    WristApp(WristApp &&) = delete;
    WristApp &operator=(WristApp &&) = delete;

    ~WristApp() = default;

    void setup();
    void loop();
    void handleIncomingImpact(const ImpactPayload &incoming);
};