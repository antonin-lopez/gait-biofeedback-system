#include "AnkleEspNowManager.h"
#include "../../include/NetworkConfig.h"
#ifdef TARGET_ANKLE
#include <WiFi.h>
#include <esp_now.h>
#include <cstring>
#endif

bool AnkleEspNowManager::init() {
#ifdef TARGET_ANKLE
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        return false;
    }

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, WRIST_HUB_MAC, sizeof(WRIST_HUB_MAC));
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    return esp_now_add_peer(&peerInfo) == ESP_OK;
#else
    return true;
#endif
}

bool AnkleEspNowManager::send(const uint8_t* peerMac, const uint8_t* data, size_t len) {
#ifdef TARGET_ANKLE
    return esp_now_send(peerMac, const_cast<uint8_t*>(data), len) == ESP_OK;
#else
    (void)peerMac;
    (void)data;
    (void)len;
    return true;
#endif
}

void AnkleEspNowManager::registerReceiveCallback(ReceiveCallback cb) {
    (void)cb;
}
