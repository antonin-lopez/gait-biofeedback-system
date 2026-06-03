#pragma once
#include <cstdint>

class Hardware
{
private:
    static uint32_t currentBgColor_;

    // Méthode privée d'aide pour dessiner les éléments communs (Fond, Header, Batterie montre et chevilles)
    // Respecte le principe DRY (Don't Repeat Yourself)
    static void prepareScreen(const char *header, int leftBat, int rightBat);

public:
    static void init();
    static void update();
    static bool isShortPress();
    static bool isLongPress();
    static float getAccelMagnitude();
    static void setBackgroundColor(uint32_t rgbColor);
    static void beep(uint32_t frequencyHz, uint32_t durationMs);

    // Surcharge 1 : Disposition "Centrée" -> Utilisée par IDLE, DIAGNOSTIC, CALIBRATION, PAUSE
    static void display(const char *header, const char *bodyCenter = "", int leftBat = -2, int rightBat = -2);

    // Surcharge 2 : Disposition "Scindée" -> Utilisée exclusivement par RUNNING (Normal & Alerte)
    static void display(const char *header, const char *bodyLeft, const char *bodyRight, int leftBat = -2, int rightBat = -2);
};