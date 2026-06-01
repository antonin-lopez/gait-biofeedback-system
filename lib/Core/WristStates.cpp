#include "WristStates.h"
#include "Feedback.h"
#include "../../include/AppConfig.h"

namespace WristStates {

void handleWristState(SystemState state, Feedback& ui, bool btnShort, bool btnLong, float currentAsymmetry) {
    (void)btnShort;
    (void)btnLong;

    switch (state) {
        case SystemState::IDLE:
            ui.setLedPattern(FeedbackColor::ORANGE_BREATH);
            break;

        case SystemState::DIAGNOSTIC:
            ui.setLedPattern(FeedbackColor::WHITE_FIXED);
            ui.triggerBuzzerBeep(1000, 100);
            break;

        case SystemState::CALIBRATION:
            ui.setLedPattern(FeedbackColor::BLUE_FLASH);
            break;

        case SystemState::RUNNING_NORMAL:
            ui.setLedPattern(FeedbackColor::GREEN_FIXED);
            break;

        case SystemState::RUNNING_ALERT:
            if (currentAsymmetry > ASYMMETRY_THRESHOLD) {
                ui.setLedPattern(FeedbackColor::RED_FLASH);
                ui.triggerBuzzerBeep(2000, 200);
            }
            break;

        case SystemState::PAUSE:
            ui.setLedPattern(FeedbackColor::ORANGE_FIXED);
            break;

        default:
            break;
    }
}

}  // namespace WristStates
