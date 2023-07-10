#pragma once
#include "Time.h"

bool isTimePassed(unsigned long interval)
{
    unsigned long currentTime = millis();
    static unsigned long lastTime = currentTime;

    if (currentTime - lastTime >= interval)
    {
        lastTime = currentTime;
        return true;
    }
    else
    {
        return false;
    }
}
