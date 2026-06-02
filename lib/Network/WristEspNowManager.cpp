#include "WristEspNowManager.h"
#include "ProtocolCodec.h"
#include "../../include/NetworkConfig.h"

#ifdef TARGET_WRIST
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <WiFi.h>
#include <esp_now.h>
#endif

WristEspNowManager *WristEspNowManager::activeInstance_ = nullptr;

void WristEspNowManager::onReceiveTaskCallback(const uint8_t *mac, const uint8_t *data, int len)
{
#ifdef TARGET_WRIST
    (void)mac;
    WristEspNowManager *const mgr = activeInstance_;
    if (!mgr || !data)
    {
        return;
    }

    if (len == static_cast<int>(IMPACT_PAYLOAD_WIRE_SIZE))
    {
        ImpactPayload payload{};
        if (deserializeImpactPayload(data, static_cast<size_t>(len), payload))
        {
            xQueueSend(static_cast<QueueHandle_t>(mgr->impactQueue_), &payload, 0);
        }
        return;
    }

    if (len == static_cast<int>(HEARTBEAT_PAYLOAD_WIRE_SIZE))
    {
        HeartbeatPayload payload{};
        if (deserializeHeartbeatPayload(data, static_cast<size_t>(len), payload))
        {
            xQueueSend(static_cast<QueueHandle_t>(mgr->heartbeatQueue_), &payload, 0);
        }
    }
#else
    (void)mac;
    (void)data;
    (void)len;
#endif
}

WristEspNowManager::~WristEspNowManager()
{
    if (activeInstance_ == this)
    {
        activeInstance_ = nullptr;
    }
#ifdef TARGET_WRIST
    if (impactQueue_)
    {
        vQueueDelete(static_cast<QueueHandle_t>(impactQueue_));
    }
    if (heartbeatQueue_)
    {
        vQueueDelete(static_cast<QueueHandle_t>(heartbeatQueue_));
    }
#endif
}

bool WristEspNowManager::init()
{
#ifdef TARGET_WRIST
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
    {
        return false;
    }

    impactQueue_ = xQueueCreate(ESP_NOW_QUEUE_SIZE, sizeof(ImpactPayload));
    heartbeatQueue_ = xQueueCreate(ESP_NOW_QUEUE_SIZE, sizeof(HeartbeatPayload));
    if (!impactQueue_ || !heartbeatQueue_)
    {
        return false;
    }

    activeInstance_ = this;
    esp_now_register_recv_cb(onReceiveTaskCallback);
    return true;
#else
    return true;
#endif
}

bool WristEspNowManager::send(const uint8_t *peerMac, const uint8_t *data, size_t len)
{
#ifdef TARGET_WRIST
    return esp_now_send(peerMac, const_cast<uint8_t *>(data), len) == ESP_OK;
#else
    (void)peerMac;
    (void)data;
    (void)len;
    return true;
#endif
}

void WristEspNowManager::registerReceiveCallback(ReceiveCallback cb)
{
    receiveCallback_ = cb;
}

bool WristEspNowManager::getNextMessage(ImpactPayload &outPayload)
{
#ifdef TARGET_WRIST
    if (!impactQueue_)
    {
        return false;
    }
    return xQueueReceive(static_cast<QueueHandle_t>(impactQueue_), &outPayload, 0) == pdTRUE;
#else
    (void)outPayload;
    return false;
#endif
}

bool WristEspNowManager::getNextHeartbeat(HeartbeatPayload &outHeartbeat)
{
#ifdef TARGET_WRIST
    if (!heartbeatQueue_)
    {
        return false;
    }
    return xQueueReceive(static_cast<QueueHandle_t>(heartbeatQueue_), &outHeartbeat, 0) == pdTRUE;
#else
    (void)outHeartbeat;
    return false;
#endif
}