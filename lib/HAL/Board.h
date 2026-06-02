#pragma once

#include <cstdint>

// Interface matérielle de la carte (bouton, batterie, veille).
class Board {
public:
    virtual ~Board() = default;
    virtual bool init() = 0;
    virtual void update() = 0;
    virtual bool isButtonPressed() const = 0;
    virtual uint8_t getBatteryLevel() const = 0;
    virtual void sleep(uint32_t durationMs) = 0;
};
