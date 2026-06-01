#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include "INetworkManager.h"
#include "../../include/Protocol.h"
#include <cstdint>

#ifdef TARGET_WRIST
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif

class EspNowManager : public INetworkManager {
private:
    static EspNowManager* _instance;
    ReceiveCallback _receiveCallback;

#ifdef TARGET_WRIST
    QueueHandle_t _messageQueue;
    static void onReceiveISR(const uint8_t* mac, const uint8_t* data, int len);
#endif

public:
    EspNowManager();
    ~EspNowManager();
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;

#ifdef TARGET_WRIST
    bool getNextMessage(ImpactPayload* outPayload);
#endif
};

#endif // ESPNOWMANAGER_H
