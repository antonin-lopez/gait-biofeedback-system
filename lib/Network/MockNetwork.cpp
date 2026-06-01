#include "MockNetwork.h"

bool MockNetwork::init() {
    return true;
}

bool MockNetwork::send(const uint8_t* peerMac, const uint8_t* data, size_t len) {
    (void)peerMac;
    (void)data;
    (void)len;
    return true;
}

void MockNetwork::registerReceiveCallback(ReceiveCallback cb) {
    receiveCallback_ = cb;
}

void MockNetwork::simulateIncomingData(const uint8_t* data, int len) {
    if (receiveCallback_) {
        const uint8_t mockMac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        receiveCallback_(mockMac, data, len);
    }
}
