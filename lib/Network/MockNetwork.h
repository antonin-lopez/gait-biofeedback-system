#pragma once

#include "NetworkManager.h"
#include <queue>

class MockNetwork : public NetworkManager {
private:
    ReceiveCallback receiveCallback_;
    std::queue<ImpactPayload> inboundQueue_;
    std::queue<HeartbeatPayload> heartbeatQueue_;

public:
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;
    bool getNextMessage(ImpactPayload& outPayload) override;
    bool getNextHeartbeat(HeartbeatPayload& outHeartbeat) override;

    void simulateIncomingData(const uint8_t* data, int len);
    void pushInbound(const ImpactPayload& payload);
    void pushHeartbeat(const HeartbeatPayload& payload);
};
