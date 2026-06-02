#pragma once

#include <cstdint>

enum class SystemState : uint8_t
{
    IDLE,           // État de veille initial
    DIAGNOSTIC,     // Vérification des capteurs
    CALIBRATION,    // Établissement de la baseline
    RUNNING_NORMAL, // Course stable
    RUNNING_ALERT,  // Course avec asymétrie > 10 %
    PAUSE,          // Pause pendant la course
    COUNT           // Permet de connaître dynamiquement le nombre total d'états
};

enum class DeviceRole : uint8_t
{
    WRIST_HUB,
    ANKLE_LEFT,
    ANKLE_RIGHT
};

enum class FootSide : uint8_t
{
    LEFT = 0,
    RIGHT = 1
};

enum class FeedbackColor : uint8_t
{
    ORANGE_BREATH,
    WHITE_FIXED,
    BLUE_FLASH,
    GREEN_FIXED,
    RED_FLASH,
    ORANGE_FIXED,
    SCREEN_BLANK
};
