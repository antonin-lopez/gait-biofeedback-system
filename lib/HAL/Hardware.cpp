#include "Hardware.h"
#include <M5Unified.h>
#include <cmath>

uint32_t Hardware::currentBgColor_ = 0x000000;

void Hardware::init()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.setRotation(3);
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

void Hardware::setBackgroundColor(uint32_t rgbColor)
{
    currentBgColor_ = rgbColor;
}

void Hardware::beep(uint32_t frequencyHz, uint32_t durationMs)
{
    M5.Speaker.tone(frequencyHz, durationMs);
}

void Hardware::display(const char *state, const char *data)
{
    M5.Lcd.fillScreen(currentBgColor_);

    uint8_t r = (currentBgColor_ >> 16) & 0xFF;
    uint8_t g = (currentBgColor_ >> 8) & 0xFF;
    uint8_t b = currentBgColor_ & 0xFF;

    // Calcul de la luminosité perçue selon la sensibilité de l'œil humain
    float brightness = (r * 0.299f) + (g * 0.587f) + (b * 0.114f);

    if (brightness > 128.0f)
    {
        M5.Lcd.setTextColor(0x000000);
    }
    else
    {
        M5.Lcd.setTextColor(0xFFFFFF);
    }

    int w = M5.Lcd.width();
    int h = M5.Lcd.height();

    int batLevel = M5.Power.getBatteryLevel();

    char batStr[8];
    snprintf(batStr, sizeof(batStr), "%d%%", batLevel);
    M5.Lcd.setTextSize(1.5);

    // Calcule dynamique de la largeur en pixels du texte (ex: "100%" prendra plus de pixels que "9%")
    int batW = M5.Lcd.textWidth(batStr);

    int margin = w * 0.06;
    int posYState = margin;

    int posXBat = w - batW - margin;
    int posYBat = margin;

    // Affichage de la batterie
    M5.Lcd.setCursor(posXBat, posYBat);
    M5.Lcd.printf("%s", batStr);

    // Affichage de la première ligne (Mode/État actuel)
    M5.Lcd.setCursor(margin, posYState);
    M5.Lcd.setTextSize(1.5);
    M5.Lcd.printf("%s", state);

    // Affichage de la deuxième ligne (Valeurs numériques : Pas, Asymétrie...)
    M5.Lcd.setTextSize(2.5);
    int dataW = M5.Lcd.textWidth(data);
    int dataH = M5.Lcd.fontHeight();
    int posXData = (w - dataW) / 2;
    int posYData = (h - dataH) / 2;
    M5.Lcd.setCursor(posXData, posYData);
    M5.Lcd.printf("%s", data);
}