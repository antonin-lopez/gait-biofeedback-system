#pragma once

#include <cstdint>

enum class SystemState : uint8_t {
    REPOS,          // État de veille initial
    DIAGNOSTIC,     // Vérification des capteurs
    CALIBRATION,    // Établissement de la baseline
    COURSE_NORMAL,  // Course stable
    COURSE_ALERTE,  // Course avec asymétrie > 10%
    PAUSE           // Pause pendant la course
};

enum class DeviceRole : uint8_t {
    WRIST_HUB,
    ANKLE_LEFT,
    ANKLE_RIGHT
};

enum class FootSide : uint8_t {
    LEFT = 0,
    RIGHT = 1
};

enum class FeedbackColor : uint8_t {
    ORANGE_BREATH,
    WHITE_FIXED,
    BLUE_FLASH,
    GREEN_FIXED,
    RED_FLASH,
    ORANGE_FIXED,
    SCREEN_BLANK
};
