#pragma once
#include <cstdint>

// ─── CONFIGURATION DES SEUILS D'IMPACT ───
#define DEFAULT_DETECTION_THRESHOLD 3.5f  // Seuil physique sur la cheville
#define DEFAULT_VALIDATION_THRESHOLD 4.0f // Seuil de validation sur le bracelet (doit toujours être égal ou supérieur au seuil de détection pour éviter les faux positifs)

enum class SystemState : uint8_t
{
    IDLE,
    DIAGNOSTIC,
    CALIBRATION,
    RUNNING_NORMAL,
    RUNNING_ALERT,
    PAUSE
};
enum class DeviceRole : uint8_t
{
    ANKLE_LEFT,
    ANKLE_RIGHT
};

#pragma pack(push, 1)
struct ImpactMessage
{
    float peakForce;
    uint8_t isLeft;
    uint32_t seqNum;
};

struct HeartbeatMessage
{
    DeviceRole role;
    uint8_t batteryLevel;
};
#pragma pack(pop)

inline const uint8_t WRIST_MAC[6] = {0xC0, 0xCD, 0xD6, 0x14, 0x9E, 0x20};