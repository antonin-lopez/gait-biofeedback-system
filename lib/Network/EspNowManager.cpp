#include "EspNowManager.h"
#include "../../include/NetworkConfig.h"
#include "ProtocolCodec.h"
#include <cstring>

#ifdef TARGET_WRIST
#include <WiFi.h>
#include <esp_now.h>
#endif

#ifdef TARGET_ANKLE
#include <WiFi.h>
#include <esp_now.h>
#endif

EspNowManager* EspNowManager::activeInstance_ = nullptr;

#ifdef TARGET_WRIST
void EspNowManager::onReceiveTaskCallback(const uint8_t* mac, const uint8_t* data, int len) {
    (void)mac;
    EspNowManager* const mgr = activeInstance_;
    if (!mgr || len != static_cast<int>(IMPACT_PAYLOAD_WIRE_SIZE)) {
        return;
    }

    ImpactPayload payload;
    if (!deserializeImpactPayload(data, static_cast<size_t>(len), payload)) {
        return;
    }

    xQueueSend(mgr->messageQueue_, &payload, 0);
}
#endif

EspNowManager::EspNowManager() : receiveCallback_(nullptr) {
#ifdef TARGET_WRIST
    messageQueue_ = nullptr;
#endif
}

EspNowManager::~EspNowManager() {
    if (activeInstance_ == this) {
        activeInstance_ = nullptr;
    }
#ifdef TARGET_WRIST
    if (messageQueue_) {
        vQueueDelete(messageQueue_);
    }
#endif
}

bool EspNowManager::registerWristPeer() {
#ifdef TARGET_ANKLE
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, WRIST_HUB_MAC, sizeof(WRIST_HUB_MAC));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        return false;
    }
#endif
    return true;
}

bool EspNowManager::init() {
#ifdef TARGET_WRIST
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    messageQueue_ = xQueueCreate(ESP_NOW_QUEUE_SIZE, sizeof(ImpactPayload));
    if (!messageQueue_) {
        return false;
    }

    activeInstance_ = this;
    esp_now_register_recv_cb(onReceiveTaskCallback);

    return true;
#elif defined(TARGET_ANKLE)
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    return registerWristPeer();
#else
    return true;
#endif
}

bool EspNowManager::send(const uint8_t* peerMac, const uint8_t* data, size_t len) {
#if defined(TARGET_WRIST) || defined(TARGET_ANKLE)
    return esp_now_send(peerMac, const_cast<uint8_t*>(data), len) == ESP_OK;
#else
    (void)peerMac;
    (void)data;
    (void)len;
    return true;
#endif
}

void EspNowManager::registerReceiveCallback(ReceiveCallback cb) {
    receiveCallback_ = cb;
}

#ifdef TARGET_WRIST
bool EspNowManager::getNextMessage(ImpactPayload& outPayload) {
    if (!messageQueue_) {
        return false;
    }

    return xQueueReceive(messageQueue_, &outPayload, 0) == pdTRUE;
}
#endif
