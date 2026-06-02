#pragma once

#include "../../include/Protocol.h"
#include <cstddef>
#include <cstdint>

// Taille fixe du buffer fil (sérialisation explicite, indépendante de l'alignement struct).
constexpr uint8_t PROTOCOL_VERSION = 1;
constexpr size_t IMPACT_PAYLOAD_WIRE_SIZE = 10;
constexpr size_t HEARTBEAT_PAYLOAD_WIRE_SIZE = 3;

size_t serializeImpactPayload(const ImpactPayload& payload, uint8_t* buffer, size_t capacity);
bool deserializeImpactPayload(const uint8_t* buffer, size_t length, ImpactPayload& out);
size_t serializeHeartbeatPayload(const HeartbeatPayload& payload, uint8_t* buffer, size_t capacity);
bool deserializeHeartbeatPayload(const uint8_t* buffer, size_t length, HeartbeatPayload& out);
