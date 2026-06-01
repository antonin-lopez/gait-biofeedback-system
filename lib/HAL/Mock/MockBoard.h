#ifndef MOCKBOARD_H
#define MOCKBOARD_H

#include "../IBoard.h"

class MockBoard : public IBoard {
public:
    bool buttonStateMock = false;
    uint8_t batteryLevelMock = 100;

    void init() override;
    bool isButtonPressed() override;
    uint8_t getBatteryLevel() override;
    void sleep(uint32_t durationMs) override;
};

#endif // MOCKBOARD_H
