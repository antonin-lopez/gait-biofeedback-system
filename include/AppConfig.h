#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <cstdint>

constexpr float ASYMMETRY_THRESHOLD = 10.0f;
constexpr uint8_t CALIBRATION_REQUIRED_STEPS = 30;
constexpr float FILTER_EMA_ALPHA = 0.2f;
constexpr uint32_t DEBOUNCE_DELAY_MS = 50;
constexpr uint32_t SAMPLING_RATE_IMU_HZ = 100;
constexpr uint32_t CALIBRATION_TIMEOUT_MS = 10000;
constexpr float IMPACT_DETECTION_THRESHOLD_G = 2.0f;

#endif // APPCONFIG_H
