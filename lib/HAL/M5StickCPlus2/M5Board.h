#ifndef M5BOARD_H
#define M5BOARD_H

#include "../IBoard.h"

class M5Board : public IBoard {
public:
    void init() override;
    bool isButtonPressed() override;
    uint8_t getBatteryLevel() override;
    void sleep(uint32_t durationMs) override;
};

#endif // M5BOARD_H
