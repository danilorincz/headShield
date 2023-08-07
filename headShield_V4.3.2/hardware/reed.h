#pragma once
#include "..\movingAverage.h"

class ReedSwitch
{
public:
    int pin;
    bool state;
    MovingAverage smooth;
    ReedSwitch(int pin, int smoothSize)
        : pin(pin),
          smooth(smoothSize)
    {
    }
    void begin()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    bool scan()
    {
        smooth.addMajority(digitalRead(pin));
        state = smooth.getMajority();
        return state;
    }
};