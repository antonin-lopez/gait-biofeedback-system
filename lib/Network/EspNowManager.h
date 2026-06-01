#pragma once

#include "NetworkManager.h"
#include "../../include/Protocol.h"
#include <cstdint>

#ifdef TARGET_WRIST
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#endif

// Gestionnaire ESP-NOW (réception via file FreeRTOS, pas en ISR matérielle).
class EspNowManager : public NetworkManager {
private:
    static EspNowManager* instance_;
    ReceiveCallback receiveCallback_;

#ifdef TARGET_WRIST
    QueueHandle_t messageQueue_;
    // Callback ESP-NOW : s'exécute dans la tâche WiFi, pas dans une ISR CPU.
    static void onReceiveTaskCallback(const uint8_t* mac, const uint8_t* data, int len);
#endif

    bool registerWristPeer();

public:
    EspNowManager();
    ~EspNowManager();
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;

#ifdef TARGET_WRIST
    bool getNextMessage(ImpactPayload& outPayload) override;
#endif
};
