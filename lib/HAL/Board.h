#pragma once

#include <cstdint>

// Interface matérielle de la carte (bouton, batterie, veille).
class Board {
public:
    virtual ~Board() = default;
    virtual void init() = 0;
    virtual bool isButtonPressed() = 0;
    virtual uint8_t getBatteryLevel() = 0;
    virtual void sleep(uint32_t durationMs) = 0;
};
