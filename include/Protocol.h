#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>

#pragma pack(push, 1)

struct ImpactPayload {
    uint32_t timestampMs;
    float peakDeceleration;
    uint8_t footSide;
    uint32_t seqNum;  // Sequence number to detect lost packets
};

struct HeartbeatPayload {
    uint8_t deviceRole;
    uint8_t batteryLevel;
};

#pragma pack(pop)

#define IMPACT_TIMEOUT_MS 1500  // If impact older than 1.5s, reset (packet lost)

#endif // PROTOCOL_H

