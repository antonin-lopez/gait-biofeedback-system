#include "M5Feedback.h"
#include <M5Unified.h>

void M5Feedback::setLedPattern(FeedbackColor color) {
    uint32_t rgbColor = 0x000000;
    switch (color) {
        case FeedbackColor::ORANGE_BREATH:
            rgbColor = 0xFF6600;
            break;
        case FeedbackColor::WHITE_FIXED:
            rgbColor = 0xFFFFFF;
            break;
        case FeedbackColor::BLUE_FLASH:
            rgbColor = 0x0066FF;
            break;
        case FeedbackColor::GREEN_FIXED:
            rgbColor = 0x00FF00;
            break;
        case FeedbackColor::RED_FLASH:
            rgbColor = 0xFF0000;
            break;
        case FeedbackColor::ORANGE_FIXED:
            rgbColor = 0xFF6600;
            break;
        default:
            break;
    }
    M5.Lcd.fillScreen(rgbColor);
}

void M5Feedback::triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs) {
    M5.Speaker.tone(frequencyHz, durationMs);
}

void M5Feedback::updateDisplay(SystemState state, float currentAsymmetry) {
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("State: %d\n", (uint8_t)state);
    M5.Lcd.printf("Asymmetry: %.1f%%\n", currentAsymmetry);
}
