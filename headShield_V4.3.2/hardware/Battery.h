#pragma once
#include "..\movingAverage.h"
class Battery
{
public:
    int pin;

    unsigned int rawValue;
    unsigned int rawValueAverage;
    unsigned int percent;
    unsigned int prevPercent;
    int level;
    MovingAverage smooth;
    float voltage;
    float a = 1;
    float b = 0;

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
    void setCorrection(float a_new, float b_new)
    {
        a = a_new;
        b = b_new;
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
        voltage = a * voltage + b;
        return voltage;
    }

    unsigned int getPercent()
    {
        float voltage = getVoltage();
        percent = map(voltage * 100, 6 * 100, 8.4 * 100, 0, 100);

        if (percent > 100)
            percent = 100;
        if (percent < 0)
            percent = 0;
        return percent;
    }
};
