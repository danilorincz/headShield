#pragma once
#include "movingAverage.h"

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
    int read()
    {
        return analogRead(pin);
    }
    int readAverage()
    {
        smooth.add(read());
        return smooth.average();
    }
    bool scan()
    {
        if (readAverage() != 4095)
            state = true;
        else
            state = false;

        return state;
    }
};
