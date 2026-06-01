#include "MockBoard.h"

void MockBoard::init() {}

bool MockBoard::isButtonPressed() {
    return simulatedButtonPressed_;
}

uint8_t MockBoard::getBatteryLevel() {
    return simulatedBatteryLevel_;
}

void MockBoard::sleep(uint32_t durationMs) {
    (void)durationMs;
}

void MockBoard::setSimulatedButtonState(bool pressed) {
    simulatedButtonPressed_ = pressed;
}

void MockBoard::setSimulatedBatteryLevel(uint8_t level) {
    simulatedBatteryLevel_ = level;
}
