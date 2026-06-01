#include <Arduino.h>
#include "M5Imu.h"
#include "EspNowManager.h"
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
