#include "Hardware.h"
#include <M5Unified.h>
#include <cmath>

void Hardware::init()
{
    auto cfg = M5.config();
    M5.begin(cfg);
}

void Hardware::update()
{
    M5.update();
}

bool Hardware::isShortPress() { return M5.BtnA.wasClicked(); }
bool Hardware::isLongPress() { return M5.BtnA.wasHold(); }

float Hardware::getAccelMagnitude()
{
    float x, y, z;
    M5.Imu.getAccelData(&x, &y, &z);
    return std::sqrt(x * x + y * y + z * z);
}

void Hardware::setLEDColor(uint32_t rgbColor)
{
    // LED Virtuelle sur l'écran LCD pour éviter d'effacer le texte
    M5.Lcd.fillRect(M5.Lcd.width() - 20, 0, 20, 20, rgbColor);
}

void Hardware::beep(uint32_t frequencyHz, uint32_t durationMs)
{
    M5.Speaker.tone(frequencyHz, durationMs);
}

void Hardware::display(const char *state, const char *data)
{
    M5.Lcd.fillRect(0, 0, M5.Lcd.width() - 22, 40, 0x000000);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("MODE: %s\n%s", state, data);
}