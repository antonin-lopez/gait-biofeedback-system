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

bool MockNetwork::getNextMessage(ImpactPayload& outPayload) {
    if (inboundQueue_.empty()) {
        return false;
    }

    outPayload = inboundQueue_.front();
    inboundQueue_.pop();
    return true;
}

bool MockNetwork::getNextHeartbeat(HeartbeatPayload& outHeartbeat) {
    if (heartbeatQueue_.empty()) {
        return false;
    }

    outHeartbeat = heartbeatQueue_.front();
    heartbeatQueue_.pop();
    return true;
}

void MockNetwork::simulateIncomingData(const uint8_t* data, int len) {
    if (receiveCallback_) {
        const uint8_t mockMac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        receiveCallback_(mockMac, data, len);
    }
}

void MockNetwork::pushInbound(const ImpactPayload& payload) {
    inboundQueue_.push(payload);
}

void MockNetwork::pushHeartbeat(const HeartbeatPayload& payload) {
    heartbeatQueue_.push(payload);
}
