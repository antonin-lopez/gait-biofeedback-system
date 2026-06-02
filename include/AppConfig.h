#pragma once

#include <cstdint>

// Seuils et constantes applicatives (pas de nombres magiques dans la logique métier).
constexpr float ASYMMETRY_THRESHOLD = 10.0f;
constexpr float ASYMMETRY_HYSTERESIS_RATIO = 0.95f;
constexpr uint8_t CALIBRATION_REQUIRED_STEPS = 30;
constexpr float FILTER_EMA_ALPHA = 0.2f;
constexpr uint32_t DEBOUNCE_DELAY_MS = 50;
constexpr uint32_t BUTTON_LONG_PRESS_MS = 800;
constexpr uint32_t IMPACT_COOLDOWN_MS = 250;
constexpr uint32_t SAMPLING_RATE_IMU_HZ = 100;
constexpr uint32_t LOOP_PERIOD_MS = 1000 / SAMPLING_RATE_IMU_HZ;
constexpr uint32_t STRIDE_PAIR_MAX_DELTA_MS = 600;
constexpr uint32_t CALIBRATION_TIMEOUT_MS = 10000;
constexpr float IMPACT_DETECTION_THRESHOLD_G = 2.0f;
constexpr float MIN_IMPACT_FORCE_G = 3.0f;
constexpr float CALIBRATION_STEP_MIN_FORCE_G = 3.0f;
constexpr uint32_t CALIBRATION_LED_PULSE_MS = 80;
constexpr uint32_t DIAGNOSTIC_LED_PULSE_MS = 80;
constexpr uint32_t DIAGNOSTIC_BEEP_HZ = 1000;
constexpr uint32_t DIAGNOSTIC_BEEP_MS = 50;
constexpr uint32_t HEARTBEAT_SEND_INTERVAL_MS = 500;
constexpr uint32_t HEARTBEAT_TIMEOUT_MS = 1500;
constexpr uint32_t HEARTBEAT_DISCONNECT_BLINK_MS = 250;
constexpr uint8_t FAULT_BLINK_COUNT = 15;
constexpr uint32_t FAULT_BLINK_DELAY_MS = 200;
constexpr uint32_t DISPLAY_REFRESH_INTERVAL_MS = 500;
