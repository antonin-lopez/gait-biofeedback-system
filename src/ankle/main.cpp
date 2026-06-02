#include <Arduino.h>
#include "M5Board.h"
#include "M5Imu.h"
#include "AnkleEspNowManager.h"
#include "AnkleApp.h"

M5Board realBoard;
M5Imu realImu;
AnkleEspNowManager realNet;
AnkleApp app(realBoard, realImu, realNet);

void setup()
{
    app.setup();
}

void loop()
{
    app.loop();
}