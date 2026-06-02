#pragma once
#include <cstdint>

class Board
{
public:
    virtual ~Board() = default;
    virtual bool init() = 0;
    virtual void update() = 0;

    virtual bool consumeShortPress() = 0;
    virtual bool consumeLongPress() = 0;

    virtual uint8_t getBatteryLevel() const = 0;
    virtual void sleep(uint32_t durationMs) = 0;
};