#pragma once
#include "Arduino.h"
#include "..\Timer.h"
#include "..\movingAverage.h"
class Tachometer
{
private:
    const int analogPin;
    unsigned long timeWhenHighStart = 0;
    unsigned long timeWhenLowStart = 0;
    unsigned long timeHigh = 0;
    unsigned long timeLow = 0;
    bool rollingValue;
    int maxMeasure = 3;
    bool lastMeasure[3];
    MovingAverage smooth;

public:
    int finalValue = -1;
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin, int smoothSize)
        : analogPin(analogPin),
          smooth(smoothSize)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT);
    }
    bool fastMajority(bool a, bool b, bool c)
    {
        return (a & b) | (b & c) | (a & c);
    }
    bool getDigital()
    {
        static bool newValue = false;
        if (analogRead(analogPin) > 0)
        {
            newValue = true;
        }
        else
        {
            newValue = false;
        }

        lastMeasure[0] = lastMeasure[1];
        lastMeasure[1] = lastMeasure[2];
        lastMeasure[2] = newValue;

        return fastMajority(lastMeasure[0], lastMeasure[1], lastMeasure[2]);
    }

    bool doMeasure()
    {
        do
        {

            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();

            while (getDigital() == lastState)
            {
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;

            dutyCycle = timeHigh + timeLow;

        } while (abs(timeHigh - timeLow) > 0.15 * dutyCycle || dutyCycle < 3500); // change this value as needed
        return true;
    }

    int getAverage()
    {
        if (doMeasure())
            smooth.add(dutyCycle);
        finalValue = smooth.average();
        return finalValue;
    }
};