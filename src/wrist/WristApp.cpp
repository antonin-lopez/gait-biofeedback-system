#include "WristApp.h"
#include "AppConfig.h"
#include <Arduino.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace {

const char* systemStateLabel(SystemState state) {
    switch (state) {
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

}  // namespace

WristApp::WristApp(Board& board, Feedback& feedback, NetworkManager& network)
    : board_(board),
      feedback_(feedback),
      network_(network),
      fsm_(idleState_, diagnosticState_, calibrationState_, runningNormalState_, runningAlertState_,
            pauseState_),
      lastLeftImpact_(0.0f),
      lastRightImpact_(0.0f),
      lastLeftImpactTime_(0),
      lastRightImpactTime_(0),
      lastLeftSeqNum_(0),
      lastRightSeqNum_(0),
      currentAsymmetry_(0.0f),
      lastButtonTime_(0),
      debouncedPressed_(false),
      lastRawPressed_(false),
      lastDebounceChangeMs_(0),
      longPressEmitted_(false),
      previousFsmState_(SystemState::IDLE),
      ledRestoreAt_(0),
      ledBasePattern_(FeedbackColor::ORANGE_BREATH),
      lastCalibrationActivityMs_(0) {
    bindStateTargets();
}

void WristApp::bindStateTargets() {
    idleState_.bindTargets(&diagnosticState_, &calibrationState_);
    diagnosticState_.bindTargets(&idleState_, &calibrationState_);
    calibrationState_.bindTargets(&idleState_, &runningNormalState_);
    runningNormalState_.bindTargets(&pauseState_, &idleState_, &runningAlertState_);
    runningAlertState_.bindTargets(&pauseState_, &idleState_, &runningNormalState_);
    pauseState_.bindTargets(&runningNormalState_, &idleState_);
}

void WristApp::handleHardwareInitFailure() {
    for (int i = 0; i < 15; ++i) {
        feedback_.setLedPattern(FeedbackColor::RED_FLASH);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    esp_restart();
}

void WristApp::setup() {
    if (!board_.init() || !network_.init()) {
        handleHardwareInitFailure();
    }
}

void WristApp::onStateEntered(SystemState entered, SystemState previous) {
    switch (entered) {
        case SystemState::CALIBRATION:
            analyzer_.resetCalibration();
            lastCalibrationActivityMs_ = millis();
            ledBasePattern_ = FeedbackColor::BLUE_FLASH;
            feedback_.setLedPattern(ledBasePattern_);
            break;

        case SystemState::DIAGNOSTIC:
            ledBasePattern_ = FeedbackColor::WHITE_FIXED;
            break;

        case SystemState::IDLE:
            if (previous == SystemState::CALIBRATION) {
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

void WristApp::updateDisplayForState(SystemState state) {
    feedback_.showStatusLine(systemStateLabel(state));
    feedback_.showAsymmetryPercent(currentAsymmetry_);
}

void WristApp::pulseLed(FeedbackColor flashColor, FeedbackColor basePattern, uint32_t durationMs) {
    ledBasePattern_ = basePattern;
    feedback_.setLedPattern(flashColor);
    ledRestoreAt_ = millis() + durationMs;
}

void WristApp::restoreLedIfNeeded() {
    if (ledRestoreAt_ == 0) {
        return;
    }

    if (static_cast<int32_t>(millis() - ledRestoreAt_) >= 0) {
        feedback_.setLedPattern(ledBasePattern_);
        ledRestoreAt_ = 0;
    }
}

void WristApp::processDiagnosticImpact() {
    pulseLed(FeedbackColor::SCREEN_BLANK, FeedbackColor::WHITE_FIXED, DIAGNOSTIC_LED_PULSE_MS);
    feedback_.triggerBuzzerBeep(1000, 50);
}

void WristApp::processCalibrationImpact(float peak, uint8_t side) {
    if (peak < CALIBRATION_STEP_MIN_FORCE_G) {
        return;
    }

    lastCalibrationActivityMs_ = millis();

    const FootSide foot =
        (side == static_cast<uint8_t>(FootSide::LEFT)) ? FootSide::LEFT : FootSide::RIGHT;
    const bool completed = analyzer_.addCalibrationStep(peak, foot);

    pulseLed(FeedbackColor::SCREEN_BLANK, FeedbackColor::BLUE_FLASH, CALIBRATION_LED_PULSE_MS);

    if (completed) {
        fsm_.requestTransition(&runningNormalState_);
    }
}

void WristApp::handleCalibrationTimeout() {
    if (fsm_.getCurrentState() != SystemState::CALIBRATION) {
        return;
    }

    if (analyzer_.getCalibrationStepCount() == 0) {
        return;
    }

    if ((millis() - lastCalibrationActivityMs_) > CALIBRATION_TIMEOUT_MS) {
        fsm_.requestTransition(&idleState_);
    }
}

bool WristApp::isImpactValid(uint32_t impactTime) const {
    if (impactTime == 0) {
        return false;
    }
    return (millis() - impactTime) < IMPACT_TIMEOUT_MS;
}

bool WristApp::isAbsoluteThresholdMet(float left, float right) const {
    return (left > MIN_IMPACT_FORCE_G && right > MIN_IMPACT_FORCE_G);
}

bool WristApp::areImpactsPairedForStride() const {
    if (!isImpactValid(lastLeftImpactTime_) || !isImpactValid(lastRightImpactTime_)) {
        return false;
    }

    if (!isAbsoluteThresholdMet(lastLeftImpact_, lastRightImpact_)) {
        return false;
    }

    const uint32_t delta = (lastLeftImpactTime_ >= lastRightImpactTime_)
                               ? (lastLeftImpactTime_ - lastRightImpactTime_)
                               : (lastRightImpactTime_ - lastLeftImpactTime_);
    return delta <= STRIDE_PAIR_MAX_DELTA_MS;
}

void WristApp::pollButton(bool& btnShort, bool& btnLong) {
    btnShort = false;
    btnLong = false;

    const bool rawPressed = board_.isButtonPressed();
    const uint32_t now = millis();

    if (rawPressed != lastRawPressed_) {
        lastDebounceChangeMs_ = now;
        lastRawPressed_ = rawPressed;
    }

    if ((now - lastDebounceChangeMs_) >= DEBOUNCE_DELAY_MS && rawPressed != debouncedPressed_) {
        debouncedPressed_ = rawPressed;

        if (debouncedPressed_) {
            lastButtonTime_ = now;
            longPressEmitted_ = false;
        } else {
            const uint32_t holdMs = now - lastButtonTime_;
            if (!longPressEmitted_ && holdMs >= DEBOUNCE_DELAY_MS && holdMs < BUTTON_LONG_PRESS_MS) {
                btnShort = true;
            }
        }
    }

    if (debouncedPressed_ && !longPressEmitted_ &&
        (now - lastButtonTime_) >= BUTTON_LONG_PRESS_MS) {
        btnLong = true;
        longPressEmitted_ = true;
    }
}

void WristApp::loop() {
    ImpactPayload incoming;
    while (network_.getNextMessage(incoming)) {
        handleIncomingImpact(incoming);
    }

    bool btnShort = false;
    bool btnLong = false;
    pollButton(btnShort, btnLong);

    if (!isImpactValid(lastLeftImpactTime_)) {
        lastLeftImpact_ = 0.0f;
    }
    if (!isImpactValid(lastRightImpactTime_)) {
        lastRightImpact_ = 0.0f;
    }

    if (areImpactsPairedForStride()) {
        currentAsymmetry_ = analyzer_.computeAsymmetry(lastLeftImpact_, lastRightImpact_);
    } else {
        currentAsymmetry_ = 0.0f;
    }

    handleCalibrationTimeout();
    fsm_.update(feedback_, btnShort, btnLong, currentAsymmetry_);

    const SystemState newState = fsm_.getCurrentState();
    if (newState != previousFsmState_) {
        onStateEntered(newState, previousFsmState_);
        previousFsmState_ = newState;
    }

    updateDisplayForState(newState);
    restoreLedIfNeeded();

    vTaskDelay(pdMS_TO_TICKS(LOOP_PERIOD_MS));
}

void WristApp::handleIncomingImpact(const ImpactPayload& incoming) {
    const SystemState state = fsm_.getCurrentState();
    const bool isLeft = (incoming.footSide == static_cast<uint8_t>(FootSide::LEFT));

    if (isLeft) {
        if (lastLeftSeqNum_ != 0 && incoming.seqNum > lastLeftSeqNum_ + 1) {
            lastLeftImpact_ = 0.0f;
            lastLeftImpactTime_ = 0;
        }
        lastLeftSeqNum_ = incoming.seqNum;
        lastLeftImpact_ = incoming.peakDeceleration;
        lastLeftImpactTime_ = millis();
    } else {
        if (lastRightSeqNum_ != 0 && incoming.seqNum > lastRightSeqNum_ + 1) {
            lastRightImpact_ = 0.0f;
            lastRightImpactTime_ = 0;
        }
        lastRightSeqNum_ = incoming.seqNum;
        lastRightImpact_ = incoming.peakDeceleration;
        lastRightImpactTime_ = millis();
    }

    if (state == SystemState::DIAGNOSTIC) {
        processDiagnosticImpact();
    } else if (state == SystemState::CALIBRATION) {
        processCalibrationImpact(incoming.peakDeceleration, incoming.footSide);
    }
}
