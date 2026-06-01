#include <Arduino.h>
#include "../../lib/HAL/M5StickCPlus2/M5Imu.h"
#include "../../lib/network/EspNowManager.h"
#include "AnkleApp.h"

M5Imu realImu;
EspNowManager realNet;
AnkleApp app(realImu, realNet);

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}
