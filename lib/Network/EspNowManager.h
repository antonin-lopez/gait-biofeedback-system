#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include "INetworkManager.h"
#include <cstdint>

class EspNowManager : public INetworkManager {
private:
    static EspNowManager* _instance;
    ReceiveCallback _receiveCallback;

public:
    EspNowManager();
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;
};

#endif // ESPNOWMANAGER_H
