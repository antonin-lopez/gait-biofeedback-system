#pragma once

#include "NetworkManager.h"

class MockNetwork : public NetworkManager {
private:
    ReceiveCallback receiveCallback_;

public:
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;

    void simulateIncomingData(const uint8_t* data, int len);
};
