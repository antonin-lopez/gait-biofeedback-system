#pragma once

#include "Board.h"

class MockBoard : public Board {
private:
    bool simulatedShortPress_ = false;
    bool simulatedLongPress_ = false;
    uint8_t simulatedBatteryLevel_ = 100;

public:
    bool init() override;
    void update() override;
    uint8_t getBatteryLevel() const override;
    void sleep(uint32_t durationMs) override;

    // Interface événementielle épurée (Plus de isButtonPressed !)
    bool consumeShortPress() override;
    bool consumeLongPress() override;

    void setSimulatedBatteryLevel(uint8_t level);
    void simulateShortPress();
    void simulateLongPress();
};