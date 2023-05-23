#pragma once
#include "Arduino.h"
#include "Timer.h"

class Tachometer
{
private:
    const int analogPin;
    unsigned long timeWhenStart = 0;
    unsigned long timeWhenFinished = 0;
    bool initialValue;
    bool rollingValue;

public:
    unsigned long dutyCycle = 0;
    Tachometer(int analogPin)
        : analogPin(analogPin)
    {
    }
    void begin()
    {
        pinMode(analogPin, INPUT);
    }

    bool getDigital()
    {
        return analogRead(analogPin) > 0;
    }
    void update()
    {
        do
        {
            initialValue = getDigital();
            rollingValue = initialValue;
            while (rollingValue == initialValue) // wait until state changes the first time
            {
                rollingValue = getDigital();
            }

            timeWhenStart = micros();
            while (rollingValue != initialValue)
            {
                rollingValue = getDigital();
            }
            while (rollingValue == initialValue)
            {
                rollingValue = getDigital();
            }
            timeWhenFinished = micros();

            dutyCycle = timeWhenFinished - timeWhenStart;
        } while (dutyCycle < 3920);
    }
};
