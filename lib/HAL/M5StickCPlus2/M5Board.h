#pragma once
#include "../Board.h"

class M5Board : public Board
{
private:
    bool shortPressPending_ = false;
    bool longPressPending_ = false;
    bool lastRawPressed_ = false;
    bool debouncedPressed_ = false;
    uint32_t lastDebounceChangeMs_ = 0;
    uint32_t lastButtonTime_ = 0;
    bool longPressEmitted_ = false;

public:
    bool init() override;
    void update() override;
    bool consumeShortPress() override;
    bool consumeLongPress() override;
    uint8_t getBatteryLevel() const override;
    void sleep(uint32_t durationMs) override;
};