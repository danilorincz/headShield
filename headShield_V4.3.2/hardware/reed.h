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
    bool readAverage()
    {
        smooth.add(digitalRead(pin));
        return smooth.average();
    }
    bool scan()
    {
        if (readAverage())
            state = true;
        else
            state = false;
        return state;
    }
};