#pragma once

#include "NetworkManager.h"

class AnkleEspNowManager : public NetworkManager {
public:
    AnkleEspNowManager() = default;
    ~AnkleEspNowManager() override = default;

    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;
};
