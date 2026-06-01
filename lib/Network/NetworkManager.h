#pragma once

#include "../../include/Protocol.h"
#include <cstddef>
#include <cstdint>

// Callback de réception ESP-NOW (contexte tâche WiFi, pas ISR matérielle).
using ReceiveCallback = void (*)(const uint8_t* mac, const uint8_t* data, int len);

// Interface d'envoi/réception réseau.
class NetworkManager {
public:
    virtual ~NetworkManager() = default;
    virtual bool init() = 0;
    virtual bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) = 0;
    virtual void registerReceiveCallback(ReceiveCallback cb) = 0;
    virtual bool getNextMessage(ImpactPayload& outPayload) {
        (void)outPayload;
        return false;
    }
};
