#include "M5Board.h"
#include <M5Unified.h>

bool M5Board::init()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    return true;
}

void M5Board::update()
{
    M5.update(); // Met à jour l'état interne des périphériques M5 Stack

    // Exploitation directe de l'anti-rebond et de la détection de durée native de M5Unified
    if (M5.BtnA.wasClicked())
    {
        shortPressPending_ = true;
    }

    if (M5.BtnA.wasHold())
    {
        longPressPending_ = true;
    }
}

bool M5Board::consumeShortPress()
{
    bool res = shortPressPending_;
    shortPressPending_ = false;
    return res;
}

bool M5Board::consumeLongPress()
{
    bool res = longPressPending_;
    longPressPending_ = false;
    return res;
}

uint8_t M5Board::getBatteryLevel() const { return M5.Power.getBatteryLevel(); }
void M5Board::sleep(uint32_t durationMs) { M5.Power.lightSleep(durationMs); }