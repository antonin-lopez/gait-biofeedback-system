#pragma once

#include <cstdint>

// Charge utile d'impact (structure logique ; le fil utilise ProtocolCodec).
struct ImpactPayload {
    float peakDeceleration;
    uint8_t footSide;
    uint32_t seqNum;  // Numéro de séquence pour détecter les paquets perdus
};

struct HeartbeatPayload {
    uint8_t deviceRole;
    uint8_t batteryLevel;
};

// Si l'impact est plus vieux que ce délai, on réinitialise (paquet perdu).
constexpr uint32_t IMPACT_TIMEOUT_MS = 1500;
