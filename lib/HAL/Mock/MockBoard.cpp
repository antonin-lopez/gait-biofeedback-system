#include "MockBoard.h"

bool MockBoard::init() {
    return true;
}

bool MockBoard::isButtonPressed() const {
    return simulatedButtonPressed_;
}

uint8_t MockBoard::getBatteryLevel() const {
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
