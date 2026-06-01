#ifndef INETWORKMANAGER_H
#define INETWORKMANAGER_H

#include <cstdint>
#include <stddef.h>

typedef void (*ReceiveCallback)(const uint8_t* mac, const uint8_t* data, int len);

struct ImpactPayload;

class INetworkManager {
public:
    virtual ~INetworkManager() = default;
    virtual bool init() = 0;
    virtual bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) = 0;
    virtual void registerReceiveCallback(ReceiveCallback cb) = 0;

    virtual bool getNextMessage(ImpactPayload* outPayload) { return false; }
};

#endif // INETWORKMANAGER_H
