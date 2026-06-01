#pragma once

#include "../Board.h"

class MockBoard : public Board {
private:
    bool simulatedButtonPressed_ = false;
    uint8_t simulatedBatteryLevel_ = 100;

public:
    void init() override;
    bool isButtonPressed() override;
    uint8_t getBatteryLevel() override;
    void sleep(uint32_t durationMs) override;

    void setSimulatedButtonState(bool pressed);
    void setSimulatedBatteryLevel(uint8_t level);
};
