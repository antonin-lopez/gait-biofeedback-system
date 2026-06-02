#include "M5Feedback.h"
#include <M5Unified.h>

void M5Feedback::setLedPattern(FeedbackColor color)
{
    uint32_t rgbColor = 0x000000;
    switch (color)
    {
    case FeedbackColor::ORANGE_BREATH:
        rgbColor = 0xFF6600;
        break;
    case FeedbackColor::WHITE_FIXED:
        rgbColor = 0xFFFFFF;
        break;
    case FeedbackColor::BLUE_FLASH:
        rgbColor = 0x0066FF;
        break;
    case FeedbackColor::GREEN_FIXED:
        rgbColor = 0x00FF00;
        break;
    case FeedbackColor::RED_FLASH:
        rgbColor = 0xFF0000;
        break;
    case FeedbackColor::ORANGE_FIXED:
        rgbColor = 0xFF6600;
        break;
    case FeedbackColor::SCREEN_BLANK:
        rgbColor = 0x000000;
        break;
    default:
        break;
    }
    // CORRIGÉ : On dessine un carré indicateur de statut (LED virtuelle) sans effacer tout le texte de l'écran
    M5.Lcd.fillRect(M5.Lcd.width() - 25, 0, 25, 25, rgbColor);
}

void M5Feedback::triggerBuzzerBeep(uint32_t frequencyHz, uint32_t durationMs)
{
    M5.Speaker.tone(frequencyHz, durationMs);
}

void M5Feedback::showStatusLine(const char *statusLine)
{
    // Nettoie uniquement la ligne de texte de statut pour éviter les superpositions graphiques
    M5.Lcd.fillRect(0, 0, M5.Lcd.width() - 26, 16, 0x000000);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.printf("%s\n", statusLine ? statusLine : "");
}

void M5Feedback::showAsymmetryPercent(float asymmetryPercent)
{
    // Nettoie uniquement la ligne dédiée aux données numériques
    M5.Lcd.fillRect(0, 16, M5.Lcd.width() - 26, 16, 0x000000);
    M5.Lcd.setCursor(0, 16);
    M5.Lcd.printf("Asym: %.1f%%\n", asymmetryPercent);
}