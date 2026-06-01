#include "EspNowManager.h"

#ifdef TARGET_WRIST
#include <WiFi.h>
#include <esp_now.h>
#endif

EspNowManager* EspNowManager::_instance = nullptr;

EspNowManager::EspNowManager() : _receiveCallback(nullptr) {}

bool EspNowManager::init() {
#ifdef TARGET_WRIST
    WiFi.mode(WIFI_STA);
    return esp_now_init() == ESP_OK;
#else
    return true;
#endif
}

bool EspNowManager::send(const uint8_t* peerMac, const uint8_t* data, size_t len) {
#ifdef TARGET_WRIST
    return esp_now_send(peerMac, (uint8_t*)data, len) == ESP_OK;
#else
    return true;
#endif
}

void EspNowManager::registerReceiveCallback(ReceiveCallback cb) {
    _receiveCallback = cb;
}
