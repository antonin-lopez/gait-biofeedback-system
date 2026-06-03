#pragma once
#include <cstdint>

class Hardware
{
private:
    static uint32_t currentBgColor_;

public:
    static void init();
    static void update();
    static bool isShortPress();
    static bool isLongPress();
    static float getAccelMagnitude();
    static void setBackgroundColor(uint32_t rgbColor);
    static void beep(uint32_t frequencyHz, uint32_t durationMs);
    static void display(const char *state, const char *data = "", int leftBat = -2, int rightBat = -2);
};