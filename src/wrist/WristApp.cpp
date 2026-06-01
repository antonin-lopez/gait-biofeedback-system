#include "WristApp.h"
#include "../../include/AppConfig.h"
#include "../../include/Protocol.h"
#include <Arduino.h>

WristApp::WristApp(Board& board, Feedback& feedback, NetworkManager& network)
    : board_(board),
      feedback_(feedback),
      network_(network),
      fsm_(reposState_, diagnosticState_, calibrationState_, courseNormalState_, courseAlerteState_,
            pauseState_),
      lastLeftImpact_(0.0f),
      lastRightImpact_(0.0f),
      lastLeftImpactTime_(0),
      lastRightImpactTime_(0),
      currentAsymmetry_(0.0f),
      lastButtonTime_(0),
      debouncedPressed_(false),
      lastRawPressed_(false),
      lastDebounceChangeMs_(0),
      longPressEmitted_(false),
      previousFsmState_(SystemState::REPOS),
      ledRestoreAt_(0),
      ledBasePattern_(FeedbackColor::ORANGE_BREATH),
      lastCalibrationActivityMs_(0) {
    bindStateTargets();
}

void WristApp::bindStateTargets() {
    reposState_.bindTargets(&diagnosticState_, &calibrationState_);
    diagnosticState_.bindTargets(&reposState_, &calibrationState_);
    calibrationState_.bindTargets(&reposState_, &courseNormalState_);
    courseNormalState_.bindTargets(&pauseState_, &reposState_);
    courseAlerteState_.bindTargets(&pauseState_, &reposState_);
    pauseState_.bindTargets(&courseNormalState_, &reposState_);
}

void WristApp::setup() {
    board_.init();
    network_.init();
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

        case SystemState::REPOS:
            if (previous == SystemState::CALIBRATION) {
                analyzer_.resetCalibration();
            }
            ledBasePattern_ = FeedbackColor::ORANGE_BREATH;
            break;

        case SystemState::COURSE_NORMAL:
            ledBasePattern_ = FeedbackColor::GREEN_FIXED;
            break;

        case SystemState::COURSE_ALERTE:
            ledBasePattern_ = FeedbackColor::RED_FLASH;
            break;

        case SystemState::PAUSE:
            ledBasePattern_ = FeedbackColor::ORANGE_FIXED;
            break;

        default:
            break;
    }
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
        fsm_.requestTransition(&courseNormalState_);
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
        fsm_.requestTransition(&reposState_);
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
        handleIncomingImpact(incoming.peakDeceleration, incoming.footSide);
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

    if (isAbsoluteThresholdMet(lastLeftImpact_, lastRightImpact_)) {
        currentAsymmetry_ = analyzer_.computeAsymmetry(lastLeftImpact_, lastRightImpact_);
    } else {
        currentAsymmetry_ = 0.0f;
    }

    const SystemState currentState = fsm_.getCurrentState();
    if (currentState == SystemState::COURSE_NORMAL && currentAsymmetry_ > ASYMMETRY_THRESHOLD) {
        fsm_.requestTransition(&courseAlerteState_);
    } else if (currentState == SystemState::COURSE_ALERTE &&
               currentAsymmetry_ <= (ASYMMETRY_THRESHOLD * ASYMMETRY_HYSTERESIS_RATIO)) {
        fsm_.requestTransition(&courseNormalState_);
    }

    handleCalibrationTimeout();
    fsm_.update(feedback_, btnShort, btnLong, currentAsymmetry_);

    const SystemState newState = fsm_.getCurrentState();
    if (newState != previousFsmState_) {
        onStateEntered(newState, previousFsmState_);
        previousFsmState_ = newState;
    }

    feedback_.updateDisplay(newState, currentAsymmetry_);
    restoreLedIfNeeded();
}

void WristApp::handleIncomingImpact(float peak, uint8_t side) {
    const SystemState state = fsm_.getCurrentState();

    if (side == static_cast<uint8_t>(FootSide::LEFT)) {
        lastLeftImpact_ = peak;
        lastLeftImpactTime_ = millis();
    } else {
        lastRightImpact_ = peak;
        lastRightImpactTime_ = millis();
    }

    if (state == SystemState::DIAGNOSTIC) {
        processDiagnosticImpact();
    } else if (state == SystemState::CALIBRATION) {
        processCalibrationImpact(peak, side);
    }
}
