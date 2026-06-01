#pragma once

#include "../../include/Protocol.h"
#include <cstddef>
#include <cstdint>

// Taille fixe du buffer fil (sérialisation explicite, indépendante de l'alignement struct).
constexpr size_t IMPACT_PAYLOAD_WIRE_SIZE = 9;

size_t serializeImpactPayload(const ImpactPayload& payload, uint8_t* buffer, size_t capacity);
bool deserializeImpactPayload(const uint8_t* buffer, size_t length, ImpactPayload& out);
