#ifndef IBOARD_H
#define IBOARD_H

#include <cstdint>

class IBoard {
public:
    virtual ~IBoard() = default;
    virtual void init() = 0;
    virtual bool isButtonPressed() = 0;
    virtual uint8_t getBatteryLevel() = 0;
    virtual void sleep(uint32_t durationMs) = 0;
};

#endif // IBOARD_H
