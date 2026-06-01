#ifndef MOCKNETWORK_H
#define MOCKNETWORK_H

#include "INetworkManager.h"

class MockNetwork : public INetworkManager {
private:
    ReceiveCallback _receiveCallback;

public:
    bool init() override;
    bool send(const uint8_t* peerMac, const uint8_t* data, size_t len) override;
    void registerReceiveCallback(ReceiveCallback cb) override;

    void simulateIncomingData(const uint8_t* data, int len);
};

#endif // MOCKNETWORK_H
