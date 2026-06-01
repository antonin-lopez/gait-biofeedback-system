#include "ProtocolCodec.h"
#include <cstring>

namespace {

// Conversion big-endian explicite (indépendante de l'endianness de la cible).
uint32_t hostToNetwork32(uint32_t value) {
    return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) |
           ((value & 0x00FF0000u) >> 8) | ((value & 0xFF000000u) >> 24);
}

uint32_t networkToHost32(uint32_t value) {
    return hostToNetwork32(value);
}

void writeUint32BE(uint8_t* buffer, uint32_t value) {
    const uint32_t networkValue = hostToNetwork32(value);
    memcpy(buffer, &networkValue, sizeof(networkValue));
}

uint32_t readUint32BE(const uint8_t* buffer) {
    uint32_t networkValue = 0;
    memcpy(&networkValue, buffer, sizeof(networkValue));
    return networkToHost32(networkValue);
}

uint32_t floatToNetworkBits(float value) {
    uint32_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    return hostToNetwork32(bits);
}

float networkBitsToFloat(uint32_t networkBits) {
    const uint32_t hostBits = networkToHost32(networkBits);
    float value = 0.0f;
    memcpy(&value, &hostBits, sizeof(value));
    return value;
}

}  // namespace

size_t serializeImpactPayload(const ImpactPayload& payload, uint8_t* buffer, size_t capacity) {
    if (!buffer || capacity < IMPACT_PAYLOAD_WIRE_SIZE) {
        return 0;
    }

    writeUint32BE(buffer, payload.timestampMs);
    writeUint32BE(buffer + 4, floatToNetworkBits(payload.peakDeceleration));
    buffer[8] = payload.footSide;
    writeUint32BE(buffer + 9, payload.seqNum);

    return IMPACT_PAYLOAD_WIRE_SIZE;
}

bool deserializeImpactPayload(const uint8_t* buffer, size_t length, ImpactPayload& out) {
    if (!buffer || length < IMPACT_PAYLOAD_WIRE_SIZE) {
        return false;
    }

    out.timestampMs = readUint32BE(buffer);
    out.peakDeceleration = networkBitsToFloat(readUint32BE(buffer + 4));
    out.footSide = buffer[8];
    out.seqNum = readUint32BE(buffer + 9);

    return true;
}
