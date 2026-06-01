#pragma once

#include "Board.h"

class M5Board : public Board {
public:
    bool init() override;
    bool isButtonPressed() const override;
    uint8_t getBatteryLevel() const override;
    void sleep(uint32_t durationMs) override;
};
