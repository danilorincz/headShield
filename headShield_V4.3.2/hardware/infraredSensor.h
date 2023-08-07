#pragma once
#include "..\movingAverage.h"

class InfraredSensor
{
public:
    int pin;
    bool state;
    MovingAverage smooth;

    InfraredSensor(int pin, int smoothSize)
        : pin(pin),
          smooth(smoothSize) // initialize MovingAverage with a size of 45
    {
    }
    int readAnalog()
    {
        return analogRead(pin);
    }
    bool readDigital()
    {
        if (readAnalog() > 4090)
            return false;
        else
            return true;
    }

    bool scan()
    {
        smooth.addMajority(readDigital());
        state = smooth.getMajority();
        return state;
    }
};
