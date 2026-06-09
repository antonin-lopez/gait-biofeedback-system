#include "Hardware.h"
#include <M5Unified.h>
#include <cmath>

namespace Hardware
{
    // Variable interne au namespace (équivalent de private)
    uint32_t currentBgColor_ = 0x000000;

    // Déclaration de la fonction d'aide interne
    void prepareScreen(const char *header, int leftBat, int rightBat)
    {
        M5.Lcd.fillScreen(currentBgColor_);

        uint8_t r = (currentBgColor_ >> 16) & 0xFF;
        uint8_t g = (currentBgColor_ >> 8) & 0xFF;
        uint8_t b = currentBgColor_ & 0xFF;
        float brightness = (r * 0.299f) + (g * 0.587f) + (b * 0.114f);
        M5.Lcd.setTextColor(brightness > 128.0f ? 0x000000 : 0xFFFFFF);

        int w = M5.Lcd.width();
        int h = M5.Lcd.height();
        int margin = w * 0.06;

        int batLevel = M5.Power.getBatteryLevel();
        char batStr[8];
        snprintf(batStr, sizeof(batStr), "%d%%", batLevel);

        M5.Lcd.setTextSize(2);
        int batW = M5.Lcd.textWidth(batStr);

        M5.Lcd.setCursor(w - batW - margin, margin);
        M5.Lcd.printf("%s", batStr);

        M5.Lcd.setCursor(margin, margin);
        M5.Lcd.printf("%s", header);

        if (leftBat != -2 || rightBat != -2)
        {
            M5.Lcd.setTextSize(2);
            int footerY = h - M5.Lcd.fontHeight() - margin;

            M5.Lcd.setCursor(margin, footerY);
            if (leftBat >= 0)
                M5.Lcd.printf("G: %d%%", leftBat);
            else
                M5.Lcd.printf("G: --");

            char rightStr[16];
            if (rightBat >= 0)
                snprintf(rightStr, sizeof(rightStr), "D: %d%%", rightBat);
            else
                snprintf(rightStr, sizeof(rightStr), "D: --");

            int rightW = M5.Lcd.textWidth(rightStr);
            M5.Lcd.setCursor(w - rightW - margin, footerY);
            M5.Lcd.printf("%s", rightStr);
        }
    }

    void init()
    {
        auto cfg = M5.config();
        M5.begin(cfg);
        M5.Lcd.setRotation(3);
    }

    void update() { M5.update(); }
    bool isShortPress() { return M5.BtnA.wasClicked(); }
    bool isLongPress() { return M5.BtnA.wasHold(); }

    float getAccelMagnitude()
    {
        float x, y, z;
        M5.Imu.getAccelData(&x, &y, &z);
        return std::sqrt(x * x + y * y + z * z);
    }

    void setBackgroundColor(uint32_t rgbColor) { currentBgColor_ = rgbColor; }
    void beep(uint32_t frequencyHz, uint32_t durationMs) { M5.Speaker.tone(frequencyHz, durationMs); }

    void display(const char *header, const char *bodyCenter, int leftBat, int rightBat)
    {
        prepareScreen(header, leftBat, rightBat);

        if (bodyCenter && bodyCenter[0] != '\0')
        {
            M5.Lcd.setTextSize(3.5);
            int dataW = M5.Lcd.textWidth(bodyCenter);
            int dataH = M5.Lcd.fontHeight();
            M5.Lcd.setCursor((M5.Lcd.width() - dataW) / 2, (M5.Lcd.height() - dataH) / 2);
            M5.Lcd.printf("%s", bodyCenter);
        }
    }

    void display(const char *header, const char *bodyLeft, const char *bodyRight, int leftBat, int rightBat)
    {
        prepareScreen(header, leftBat, rightBat);

        M5.Lcd.setTextSize(2.5);
        int bodyH = M5.Lcd.fontHeight();
        int posYBody = (M5.Lcd.height() - bodyH) / 2;
        int margin = M5.Lcd.width() * 0.06;

        if (bodyLeft && bodyLeft[0] != '\0')
        {
            M5.Lcd.setCursor(margin, posYBody);
            M5.Lcd.printf("%s", bodyLeft);
        }

        if (bodyRight && bodyRight[0] != '\0')
        {
            int rightW = M5.Lcd.textWidth(bodyRight);
            M5.Lcd.setCursor(M5.Lcd.width() - rightW - margin, posYBody);
            M5.Lcd.printf("%s", bodyRight);
        }
    }
}