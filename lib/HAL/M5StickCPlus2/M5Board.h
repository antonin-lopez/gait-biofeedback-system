#pragma once

#include "../Board.h"

class M5Board : public Board {
public:
    void init() override;
    bool isButtonPressed() override;
    uint8_t getBatteryLevel() override;
    void sleep(uint32_t durationMs) override;
};
