#include "EspNowManager.h"

#ifdef TARGET_WRIST
#include <WiFi.h>
#include <esp_now.h>
#endif

EspNowManager* EspNowManager::_instance = nullptr;

#ifdef TARGET_WRIST
void EspNowManager::onReceiveISR(const uint8_t* mac, const uint8_t* data, int len) {
    if (!EspNowManager::_instance || len != sizeof(ImpactPayload)) {
        return;
    }

    ImpactPayload payload;
    memcpy(&payload, data, sizeof(ImpactPayload));

    // NOT an ISR context — it's WiFi task context, so use standard xQueueSend, not FromISR
    xQueueSend(EspNowManager::_instance->_messageQueue, &payload, 0);
}
#endif

EspNowManager::EspNowManager()
    : _receiveCallback(nullptr) {
#ifdef TARGET_WRIST
    _messageQueue = nullptr;
#endif
}

EspNowManager::~EspNowManager() {
#ifdef TARGET_WRIST
    if (_messageQueue) {
        vQueueDelete(_messageQueue);
    }
#endif
}

bool EspNowManager::init() {
#ifdef TARGET_WRIST
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        return false;
    }

    _messageQueue = xQueueCreate(32, sizeof(ImpactPayload));
    if (!_messageQueue) {
        return false;
    }

    _instance = this;
    esp_now_register_recv_cb(onReceiveISR);

    return true;
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

#ifdef TARGET_WRIST
bool EspNowManager::getNextMessage(ImpactPayload* outPayload) {
    if (!_messageQueue || !outPayload) {
        return false;
    }

    return xQueueReceive(_messageQueue, outPayload, 0) == pdTRUE;
}
#endif


