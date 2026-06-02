#include "M5Board.h"
#include <M5Unified.h>
#include "../../include/AppConfig.h"

bool M5Board::init()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    return true;
}

void M5Board::update()
{
    M5.update();
    bool rawPressed = M5.BtnA.isPressed();
    uint32_t now = millis();

    if (rawPressed != lastRawPressed_)
    {
        lastDebounceChangeMs_ = now;
        lastRawPressed_ = rawPressed;
    }

    if ((now - lastDebounceChangeMs_) >= DEBOUNCE_DELAY_MS && rawPressed != debouncedPressed_)
    {
        debouncedPressed_ = rawPressed;
        if (debouncedPressed_)
        {
            lastButtonTime_ = now;
            longPressEmitted_ = false;
        }
        else
        {
            uint32_t holdMs = now - lastButtonTime_;
            if (!longPressEmitted_ && holdMs >= DEBOUNCE_DELAY_MS && holdMs < BUTTON_LONG_PRESS_MS)
            {
                shortPressPending_ = true;
            }
        }
    }

    if (debouncedPressed_ && !longPressEmitted_ && (now - lastButtonTime_) >= BUTTON_LONG_PRESS_MS)
    {
        longPressPending_ = true;
        longPressEmitted_ = true;
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