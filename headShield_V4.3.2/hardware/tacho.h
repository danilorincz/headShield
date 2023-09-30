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
    bool warning = true;
    int finalValue = -1;
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin, int smoothSize)
        : analogPin(analogPin),
          smooth(smoothSize)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT_PULLDOWN);
    }
    void warningOn()
    {
        warning = true;
    }
    void warningOff()
    {
        warning = false;
    }
    bool fastMajority(bool a, bool b, bool c)
    {
        return (a & b) | (b & c) | (a & c);
    }
    int getAnalog()
    {
        return analogRead(analogPin);
    }

    bool getDigital()
    {
        static bool newValue = false;
        if (getAnalog() > 0)
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
        int counter_doWhile = 0;
        int counter_while = 0;
        bool returnValue = false;

        while (true)
        {
            counter_doWhile++;

            bool lastState = getDigital();
            rollingValue = lastState;

            if (rollingValue)
                timeWhenHighStart = micros();
            else
                timeWhenLowStart = micros();
            while (getDigital() == lastState)
            {
                counter_while++;
                if (counter_while >= 200)
                    break;
            }

            if (lastState)
                timeHigh = micros() - timeWhenHighStart;
            else
                timeLow = micros() - timeWhenLowStart;

            if (counter_doWhile >= 8 || counter_while >= 200)
            {
                returnValue = false;
                break;
            }

            dutyCycle = timeHigh + timeLow;
            if (abs(timeHigh - timeLow) < 0.10 * dutyCycle && dutyCycle > 3300) //* GOOD
            {
                returnValue = true;
                break;
            }
        }

        return returnValue;
    }

    int getAverage()
    {
        if (doMeasure())
            smooth.add(dutyCycle);
        else
        {
            Serial.println("Measurement failed");
        }

        finalValue = smooth.average();
        return finalValue;
    }
};