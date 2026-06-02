#pragma once
#include <cstdint>

enum class SystemState : uint8_t { IDLE, DIAGNOSTIC, CALIBRATION, RUNNING_NORMAL, RUNNING_ALERT, PAUSE };
enum class DeviceRole : uint8_t  { ANKLE_LEFT, ANKLE_RIGHT };

#pragma pack(push, 1)
struct ImpactMessage {
    float peakForce;
    uint8_t isLeft;
    uint32_t seqNum;
};

struct HeartbeatMessage {
    DeviceRole role;
    uint8_t batteryLevel;
};
#pragma pack(pop)

inline const uint8_t WRIST_MAC[6] = {0x24, 0x0A, 0xC4, 0x12, 0x34, 0x56}; // À adapter avec la vraie adresse MAC du maître