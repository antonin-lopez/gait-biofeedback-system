#include <Arduino.h>
#include "M5Imu.h"
#include "AnkleEspNowManager.h"
#include "AnkleApp.h"

M5Imu realImu;
AnkleEspNowManager realNet;
AnkleApp app(realImu, realNet);

void setup() {
    app.setup();
}

void loop() {
    app.loop();
}
