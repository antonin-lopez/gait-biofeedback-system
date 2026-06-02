#include "ProtocolCodec.h"
#include <cstring>

namespace {

static_assert(sizeof(float) == sizeof(uint32_t), "Float size mismatch");

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
uint32_t hostToNetwork32(uint32_t value) {
    return value;
}
uint32_t networkToHost32(uint32_t value) {
    return value;
}
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
uint32_t hostToNetwork32(uint32_t value) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(value);
#else
    return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) |
           ((value & 0x00FF0000u) >> 8) | ((value & 0xFF000000u) >> 24);
#endif
}
uint32_t networkToHost32(uint32_t value) {
    return hostToNetwork32(value);
}
#else
// ESP32, Windows et la plupart des cibles de test native sont little-endian.
uint32_t hostToNetwork32(uint32_t value) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_bswap32(value);
#else
    return ((value & 0x000000FFu) << 24) | ((value & 0x0000FF00u) << 8) |
           ((value & 0x00FF0000u) >> 8) | ((value & 0xFF000000u) >> 24);
#endif
}
uint32_t networkToHost32(uint32_t value) {
    return hostToNetwork32(value);
}
#endif

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

    buffer[0] = PROTOCOL_VERSION;
    writeUint32BE(buffer + 1, floatToNetworkBits(payload.peakDeceleration));
    buffer[5] = static_cast<uint8_t>(payload.footSide);
    writeUint32BE(buffer + 6, payload.seqNum);

    return IMPACT_PAYLOAD_WIRE_SIZE;
}

bool deserializeImpactPayload(const uint8_t* buffer, size_t length, ImpactPayload& out) {
    if (!buffer || length < IMPACT_PAYLOAD_WIRE_SIZE) {
        return false;
    }

    if (buffer[0] != PROTOCOL_VERSION) {
        return false;
    }

    out.peakDeceleration = networkBitsToFloat(readUint32BE(buffer + 1));
    out.footSide = static_cast<FootSide>(buffer[5]);
    out.seqNum = readUint32BE(buffer + 6);

    return true;
}

size_t serializeHeartbeatPayload(const HeartbeatPayload& payload, uint8_t* buffer, size_t capacity) {
    if (!buffer || capacity < HEARTBEAT_PAYLOAD_WIRE_SIZE) {
        return 0;
    }

    buffer[0] = PROTOCOL_VERSION;
    buffer[1] = static_cast<uint8_t>(payload.deviceRole);
    buffer[2] = payload.batteryLevel;
    return HEARTBEAT_PAYLOAD_WIRE_SIZE;
}

bool deserializeHeartbeatPayload(const uint8_t* buffer, size_t length, HeartbeatPayload& out) {
    if (!buffer || length < HEARTBEAT_PAYLOAD_WIRE_SIZE) {
        return false;
    }
    if (buffer[0] != PROTOCOL_VERSION) {
        return false;
    }

    out.deviceRole = static_cast<DeviceRole>(buffer[1]);
    out.batteryLevel = buffer[2];
    return true;
}
