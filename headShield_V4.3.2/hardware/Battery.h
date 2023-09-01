#pragma once
#include "..\movingAverage.h"
class Battery
{
public:
    int pin;

    unsigned int rawValue;
    unsigned int rawValueAverage;
    unsigned int percent;
    float voltage;
    int level;
    MovingAverage smooth;

    // defining the two known points
    float voltage1 = 6;
    int analog1 = 2461;

    float voltage2 = 8.3;
    int analog2 = 3814;

    // calculating the slope and intercept
    float slope = (voltage2 - voltage1) / (float)(analog2 - analog1);
    float intercept = voltage1 - slope * analog1;

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
        rawValueAverage = smooth.average();
        return rawValueAverage;
    }

    float getVoltage()
    {
        voltage = slope * readAverage() + intercept;
        return voltage;
    }

    int getPercent()
    {
        float voltage = getVoltage();
        percent = map(voltage * 100, 6 * 100, 8.4 * 100, 0, 100);
        /*
        if (percent > 100)
            percent = 100;
        else if (percent < 1)
            percent = 1;*/
        return percent;
    }
};
