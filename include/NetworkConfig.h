#pragma once

#include <cstdint>

// CORRIGÉ : Ajout du mot-clé 'inline' (Norme C++17) pour garantir l'unicité
// de l'instance mémoire globale du tableau à travers tout le projet.
inline constexpr uint8_t WRIST_HUB_MAC[6] = {0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56};
inline constexpr uint32_t ESP_NOW_QUEUE_SIZE = 32;