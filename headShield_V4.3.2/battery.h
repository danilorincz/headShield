#pragma once
#include "movingAverage.h"
class Battery
{
public:
    int pin;

    unsigned int rawValue;
    unsigned int percent;
    unsigned int charge;
    int level;
    MovingAverage smooth;

    Battery(int pin, int smoothSize)
        : pin(pin),
          smooth(smoothSize)
    {
    }
    void begin()
    {
        pinMode(pin, INPUT);
    }
    int getRaw()
    {
        rawValue = analogRead(pin);
        return rawValue;
    }

    int readAverage()
    {
        smooth.add(getRaw());
        return smooth.average();
    }
    int getPercent()
    {
        percent = map(readAverage(), 1000, 4096, 0, 100);
        return percent;
    }
};
