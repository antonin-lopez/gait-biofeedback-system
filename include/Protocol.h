#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

#pragma pack(push, 1)

struct ImpactPayload {
    uint32_t timestampMs;
    float peakDeceleration;
    uint8_t footSide;
};

struct HeartbeatPayload {
    uint8_t deviceRole;
    uint8_t batteryLevel;
};

#pragma pack(pop)

#endif // PROTOCOL_H
