#include "M5Board.h"
#include <M5Unified.h>

void M5Board::init() {
    auto cfg = M5.config();
    M5.begin(cfg);
}

bool M5Board::isButtonPressed() {
    return M5.BtnA.wasPressed();
}

uint8_t M5Board::getBatteryLevel() {
    return M5.Power.getBatteryLevel();
}

void M5Board::sleep(uint32_t durationMs) {
    M5.Power.lightSleep(durationMs);
}
