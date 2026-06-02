#include "MockBoard.h"

bool MockBoard::init()
{
    return true;
}

void MockBoard::update() {}

bool MockBoard::isButtonPressed() const
{
    return simulatedButtonPressed_;
}

uint8_t MockBoard::getBatteryLevel() const
{
    return simulatedBatteryLevel_;
}

void MockBoard::sleep(uint32_t durationMs)
{
    (void)durationMs;
}

void MockBoard::setSimulatedButtonState(bool pressed)
{
    simulatedButtonPressed_ = pressed;
}

void MockBoard::setSimulatedBatteryLevel(uint8_t level)
{
    simulatedBatteryLevel_ = level;
}

// Implémentation des fonctions de consommation d'événements
bool MockBoard::consumeShortPress()
{
    bool press = simulatedShortPress_;
    simulatedShortPress_ = false;
    return press;
}

bool MockBoard::consumeLongPress()
{
    bool press = simulatedLongPress_;
    simulatedLongPress_ = false;
    return press;
}

void MockBoard::simulateShortPress()
{
    simulatedShortPress_ = true;
}

void MockBoard::simulateLongPress()
{
    simulatedLongPress_ = true;
}