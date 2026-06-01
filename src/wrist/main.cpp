#include <Arduino.h>
#include "../../lib/HAL/M5StickCPlus2/M5Board.h"
#include "../../lib/HAL/M5StickCPlus2/M5Feedback.h"
#include "../../lib/Network/EspNowManager.h"
#include "WristApp.h"

M5Board realBoard;
M5Feedback realUi;
EspNowManager realNet;
WristApp app(&realBoard, &realUi, &realNet);

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}

