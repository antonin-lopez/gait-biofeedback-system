#include <Arduino.h>
#include "M5Board.h"
#include "M5Feedback.h"
#include "WristEspNowManager.h"
#include "WristApp.h"

M5Board realBoard;
M5Feedback realUi;
WristEspNowManager realNet;
WristApp app(realBoard, realUi, realNet);

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}
