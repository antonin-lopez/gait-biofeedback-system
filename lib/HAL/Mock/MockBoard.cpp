#include "MockBoard.h"

void MockBoard::init() {
}

bool MockBoard::isButtonPressed() {
    return buttonStateMock;
}

uint8_t MockBoard::getBatteryLevel() {
    return batteryLevelMock;
}

void MockBoard::sleep(uint32_t durationMs) {
}
