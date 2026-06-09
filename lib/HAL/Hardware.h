#pragma once
#include <cstdint>

namespace Hardware
{
    // Initialisation et cycle de vie
    void init();
    void update();
    
    // Entrées utilisateur
    bool isShortPress();
    bool isLongPress();
    
    // Capteurs et Actionneurs
    float getAccelMagnitude();
    void setBackgroundColor(uint32_t rgbColor);
    void beep(uint32_t frequencyHz, uint32_t durationMs);

    // Surcharge 1 : Disposition "Centrée" -> IDLE, DIAGNOSTIC, CALIBRATION, PAUSE
    void display(const char *header, const char *bodyCenter = "", int leftBat = -2, int rightBat = -2);

    // Surcharge 2 : Disposition "Scindée" -> RUNNING (Normal & Alerte)
    void display(const char *header, const char *bodyLeft, const char *bodyRight, int leftBat = -2, int rightBat = -2);
}