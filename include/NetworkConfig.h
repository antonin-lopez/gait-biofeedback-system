#pragma once

#include <cstdint>

// MAC du hub poignet — à renseigner après appairage (WiFi.macAddress() sur le module poignet).
// Évite le broadcast FF:FF:FF:FF:FF:FF qui provoque des collisions entre coureurs.
constexpr uint8_t WRIST_HUB_MAC[6] = {0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56};
constexpr uint32_t ESP_NOW_QUEUE_SIZE = 32;
