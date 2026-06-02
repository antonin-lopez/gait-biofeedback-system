#pragma once
#include "NetworkManager.h"

struct QueueDefinition;
using QueueHandle_t = QueueDefinition *;

namespace BioGait
{

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

        // Suppression explicite des opérations de copie et déplacement (Sécurité RTOS)
        WristEspNowManager(const WristEspNowManager &) = delete;
        WristEspNowManager &operator=(const WristEspNowManager &) = delete;
        WristEspNowManager(WristEspNowManager &&) = delete;
        WristEspNowManager &operator=(WristEspNowManager &&) = delete;

        bool init() override;
        bool send(const uint8_t *peerMac, const uint8_t *data, size_t len) override;
        void registerReceiveCallback(ReceiveCallback cb) override;
        bool getNextMessage(ImpactPayload &outPayload) override;
        bool getNextHeartbeat(HeartbeatPayload &outHeartbeat) override;
    };

} // namespace BioGait