#include "M5Board.h"
#include <M5Unified.h>

bool M5Board::init() {
    auto cfg = M5.config();
    M5.begin(cfg);
    return true;
}

bool M5Board::isButtonPressed() const {
    M5.update();
    return M5.BtnA.isPressed();
}

uint8_t M5Board::getBatteryLevel() const {
    return M5.Power.getBatteryLevel();
}

void M5Board::sleep(uint32_t durationMs) {
    M5.Power.lightSleep(durationMs);
}
