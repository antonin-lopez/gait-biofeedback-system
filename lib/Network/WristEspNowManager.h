#pragma once
#include "NetworkManager.h"

// Déclaration anticipée opaque (Strong typing)
struct QueueDefinition;
using QueueHandle_t = QueueDefinition *;

class WristEspNowManager : public NetworkManager
{
private:
    static WristEspNowManager *activeInstance_;
    QueueHandle_t impactQueue_ = nullptr;
    QueueHandle_t heartbeatQueue_ = nullptr;
    ReceiveCallback receiveCallback_ = nullptr;

    static void onReceiveTaskCallback(const uint8_t *mac, const uint8_t *data, int len);

public:
    WristEspNowManager() = default;
    ~WristEspNowManager() override;

    bool init() override;
    bool send(const uint8_t *peerMac, const uint8_t *data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;
    bool getNextMessage(ImpactPayload &outPayload) override;
    bool getNextHeartbeat(HeartbeatPayload &outHeartbeat) override;
};