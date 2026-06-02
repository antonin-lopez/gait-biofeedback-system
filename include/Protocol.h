#pragma once

#include "Types.h"
#include <cstdint>

// Charge utile d'impact (structure logique ; le fil utilise ProtocolCodec).
struct ImpactPayload {
    float peakDeceleration;
    FootSide footSide;
    uint32_t seqNum;  // Numéro de séquence pour détecter les paquets perdus
};

struct HeartbeatPayload {
    DeviceRole deviceRole;
    uint8_t batteryLevel;
};

// Si l'impact est plus vieux que ce délai, on réinitialise (paquet perdu).
constexpr uint32_t IMPACT_TIMEOUT_MS = 1500;
