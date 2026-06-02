#include "WristApp.h"
#include "AppConfig.h"
#include <Arduino.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace
{

    const char *systemStateLabel(SystemState state)
    {
        switch (state)
        {
        case SystemState::IDLE:
            return "IDLE";
        case SystemState::DIAGNOSTIC:
            return "DIAGNOSTIC";
        case SystemState::CALIBRATION:
            return "CALIBRATION";
        case SystemState::RUNNING_NORMAL:
            return "RUNNING";
        case SystemState::RUNNING_ALERT:
            return "ALERT";
        case SystemState::PAUSE:
            return "PAUSE";
        default:
            return "UNKNOWN";
        }
    }

} // namespace

WristApp::WristApp(Board &board, Feedback &feedback, NetworkManager &network)
    : board_(board),
      feedback_(feedback),
      network_(network),
      fsm_()
{ // Le constructeur de FSM est maintenant découplé et vide
    registerStatesInFsm();
}

void WristApp::registerStatesInFsm()
{
    fsm_.registerState(&idleState_);
    fsm_.registerState(&diagnosticState_);
    fsm_.registerState(&calibrationState_);
    fsm_.registerState(&runningNormalState_);
    fsm_.registerState(&runningAlertState_);
    fsm_.registerState(&pauseState_);
}

void WristApp::handleHardwareInitFailure()
{
    for (uint8_t i = 0; i < FAULT_BLINK_COUNT; ++i)
    {
        feedback_.setLedPattern(FeedbackColor::RED_FLASH);
        vTaskDelay(pdMS_TO_TICKS(FAULT_BLINK_DELAY_MS));
    }
    esp_restart();
}

void WristApp::setup()
{
    if (!board_.init() || !network_.init())
    {
        handleHardwareInitFailure();
    }
    fsm_.setInitialState(SystemState::IDLE, feedback_);

#ifdef TARGET_WRIST
    xLastWakeTime_ = xTaskGetTickCount();
#endif
}

void WristApp::onStateEntered(SystemState entered, SystemState previous)
{
    switch (entered)
    {
    case SystemState::CALIBRATION:
        analyzer_.resetCalibration();
        lastCalibrationActivityMs_ = millis();
        ledBasePattern_ = FeedbackColor::BLUE_FLASH;
        break;

    case SystemState::DIAGNOSTIC:
        ledBasePattern_ = FeedbackColor::WHITE_FIXED;
        break;

    case SystemState::IDLE:
        if (previous == SystemState::CALIBRATION)
        {
            analyzer_.resetCalibration();
        }
        ledBasePattern_ = FeedbackColor::ORANGE_BREATH;
        break;

    case SystemState::RUNNING_NORMAL:
        ledBasePattern_ = FeedbackColor::GREEN_FIXED;
        break;

    case SystemState::RUNNING_ALERT:
        ledBasePattern_ = FeedbackColor::RED_FLASH;
        break;

    case SystemState::PAUSE:
        ledBasePattern_ = FeedbackColor::ORANGE_FIXED;
        break;

    default:
        break;
    }
}

void WristApp::updateDisplayForState(SystemState state, bool stateChanged)
{
    const uint32_t now = millis();
    const bool asymmetryChanged = (currentAsymmetry_ != lastDisplayedAsymmetry_);
    const bool refreshDue = (now - lastDisplayUpdateMs_) >= DISPLAY_REFRESH_INTERVAL_MS;

    if (stateChanged)
    {
        feedback_.showStatusLine(systemStateLabel(state));
    }

    if (stateChanged || asymmetryChanged || refreshDue)
    {
        feedback_.showAsymmetryPercent(currentAsymmetry_);
        lastDisplayedAsymmetry_ = currentAsymmetry_;
        lastDisplayUpdateMs_ = now;
    }
}

void WristApp::pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs)
{
    ledBasePattern_ = basePattern;
    feedback_.setLedPattern(flashColor);
    ledRestoreAt_ = millis() + durationMs;
}

void WristApp::restoreLedIfNeeded()
{
    if (ledRestoreAt_ == 0)
    {
        return;
    }

    if (static_cast<int32_t>(millis() - ledRestoreAt_) >= 0)
    {
        feedback_.setLedPattern(ledBasePattern_);
        ledRestoreAt_ = 0;
    }
}

void WristApp::processDiagnosticImpact()
{
    pulseLed(FeedbackColor::SCREEN_BLANK, FeedbackColor::WHITE_FIXED, DIAGNOSTIC_LED_PULSE_MS);
    feedback_.triggerBuzzerBeep(DIAGNOSTIC_BEEP_HZ, DIAGNOSTIC_BEEP_MS);
}

void WristApp::processCalibrationImpact(float peak, FootSide side)
{
    if (peak < CALIBRATION_STEP_MIN_FORCE_G)
    {
        return;
    }

    lastCalibrationActivityMs_ = millis();

    const bool completed = analyzer_.addCalibrationStep(peak, side);

    pulseLed(FeedbackColor::SCREEN_BLANK, FeedbackColor::BLUE_FLASH, CALIBRATION_LED_PULSE_MS);

    if (completed)
    {
        fsm_.requestTransition(SystemState::RUNNING_NORMAL);
    }
}

void WristApp::handleCalibrationTimeout()
{
    if (fsm_.getCurrentState() != SystemState::CALIBRATION)
    {
        return;
    }

    if (analyzer_.getCalibrationStepCount() == 0)
    {
        return;
    }

    if ((millis() - lastCalibrationActivityMs_) > CALIBRATION_TIMEOUT_MS)
    {
        fsm_.requestTransition(SystemState::IDLE);
    }
}

bool WristApp::isImpactValid(uint32_t impactTime) const
{
    if (impactTime == 0)
    {
        return false;
    }
    return (millis() - impactTime) < IMPACT_TIMEOUT_MS;
}

bool WristApp::isHeartbeatAlive(uint32_t heartbeatTime) const
{
    if (heartbeatTime == 0)
    {
        return false;
    }
    return (millis() - heartbeatTime) < HEARTBEAT_TIMEOUT_MS;
}

bool WristApp::areBothAnklesConnected() const
{
    return isHeartbeatAlive(lastLeftHeartbeatMs_) && isHeartbeatAlive(lastRightHeartbeatMs_);
}

bool WristApp::isAbsoluteThresholdMet(float left, float right) const
{
    return (left > MIN_IMPACT_FORCE_G && right > MIN_IMPACT_FORCE_G);
}

bool WristApp::areImpactsPairedForStride() const
{
    if (!isImpactValid(lastLeftImpactTime_) || !isImpactValid(lastRightImpactTime_))
    {
        return false;
    }

    if (!isAbsoluteThresholdMet(lastLeftImpact_, lastRightImpact_))
    {
        return false;
    }

    const uint32_t delta = (lastLeftImpactTime_ >= lastRightImpactTime_)
                               ? (lastLeftImpactTime_ - lastRightImpactTime_)
                               : (lastRightImpactTime_ - lastLeftImpactTime_);
    return delta <= STRIDE_PAIR_MAX_DELTA_MS;
}

void WristApp::handleHeartbeatTimeoutUi()
{
    if (areBothAnklesConnected())
    {
        heartbeatBlinkOn_ = false;
        return;
    }

    if ((millis() - lastHeartbeatBlinkMs_) < HEARTBEAT_DISCONNECT_BLINK_MS)
    {
        return;
    }

    lastHeartbeatBlinkMs_ = millis();
    heartbeatBlinkOn_ = !heartbeatBlinkOn_;
    feedback_.setLedPattern(heartbeatBlinkOn_ ? FeedbackColor::WHITE_FIXED : ledBasePattern_);
}

void WristApp::loop()
{
    board_.update();

    ImpactPayload incoming;
    bool newImpactReceived = false;
    while (network_.getNextMessage(incoming))
    {
        handleIncomingImpact(incoming);
        newImpactReceived = true;
    }

    HeartbeatPayload heartbeat;
    while (network_.getNextHeartbeat(heartbeat))
    {
        const uint32_t now = millis();
        if (heartbeat.deviceRole == DeviceRole::ANKLE_LEFT)
        {
            lastLeftHeartbeatMs_ = now;
        }
        else if (heartbeat.deviceRole == DeviceRole::ANKLE_RIGHT)
        {
            lastRightHeartbeatMs_ = now;
        }
    }

    if (!isImpactValid(lastLeftImpactTime_))
    {
        lastLeftImpact_ = 0.0f;
    }
    if (!isImpactValid(lastRightImpactTime_))
    {
        lastRightImpact_ = 0.0f;
    }

    // LOGIQUE ÉVÉNEMENTIELLE
    if (newImpactReceived && areBothAnklesConnected() && areImpactsPairedForStride())
    {
        currentAsymmetry_ = analyzer_.computeAsymmetry(lastLeftImpact_, lastRightImpact_);
    }
    else if (!areBothAnklesConnected())
    {
        currentAsymmetry_ = 0.0f;

        // CORRIGÉ : Si une cheville se déconnecte pendant l'effort, on bascule de sécurité en PAUSE.
        // Cela évite que la FSM valide une course "normale" à cause d'une asymétrie tombée à 0.
        SystemState currentState = fsm_.getCurrentState();
        if (currentState == SystemState::RUNNING_NORMAL || currentState == SystemState::RUNNING_ALERT)
        {
            fsm_.requestTransition(SystemState::PAUSE);
        }
    }

    handleCalibrationTimeout();

    SystemContext ctx;
    ctx.btnShort = board_.consumeShortPress();
    ctx.btnLong = board_.consumeLongPress();
    ctx.asymmetry = currentAsymmetry_;

    fsm_.update(feedback_, ctx);

    const SystemState newState = fsm_.getCurrentState();
    const bool stateChanged = (newState != previousFsmState_);
    if (stateChanged)
    {
        onStateEntered(newState, previousFsmState_);
        previousFsmState_ = newState;
    }

    updateDisplayForState(newState, stateChanged);
    restoreLedIfNeeded();
    handleHeartbeatTimeoutUi();

#ifdef TARGET_WRIST
    vTaskDelayUntil(&xLastWakeTime_, pdMS_TO_TICKS(LOOP_PERIOD_MS));
#else
    vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
#endif
}

void WristApp::handleIncomingImpact(const ImpactPayload &incoming)
{
    const SystemState state = fsm_.getCurrentState();
    const bool isLeft = (incoming.footSide == FootSide::LEFT);

    if (isLeft)
    {
        const uint32_t delta = incoming.seqNum - lastLeftSeqNum_;
        const bool gapDetected = (lastLeftSeqNum_ != UINT32_MAX) && (delta > 1);
        lastLeftSeqNum_ = incoming.seqNum;
        lastLeftImpact_ = incoming.peakDeceleration;
        lastLeftImpactTime_ = millis();
        if (gapDetected)
        {
            lastRightImpact_ = 0.0f;
            lastRightImpactTime_ = 0;
        }
    }
    else
    {
        const uint32_t delta = incoming.seqNum - lastRightSeqNum_;
        const bool gapDetected = (lastRightSeqNum_ != UINT32_MAX) && (delta > 1);
        lastRightSeqNum_ = incoming.seqNum;
        lastRightImpact_ = incoming.peakDeceleration;
        lastRightImpactTime_ = millis();
        if (gapDetected)
        {
            lastLeftImpact_ = 0.0f;
            lastLeftImpactTime_ = 0;
        }
    }

    if (state == SystemState::DIAGNOSTIC)
    {
        processDiagnosticImpact();
    }
    else if (state == SystemState::CALIBRATION)
    {
        processCalibrationImpact(incoming.peakDeceleration, incoming.footSide);
    }
}