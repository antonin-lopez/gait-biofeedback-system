#include "MockNetwork.h"

bool MockNetwork::init() {
    return true;
}

bool MockNetwork::send(const uint8_t* peerMac, const uint8_t* data, size_t len) {
    return true;
}

void MockNetwork::registerReceiveCallback(ReceiveCallback cb) {
    _receiveCallback = cb;
}

void MockNetwork::simulateIncomingData(const uint8_t* data, int len) {
    if (_receiveCallback) {
        uint8_t mockMac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        _receiveCallback(mockMac, data, len);
    }
}
