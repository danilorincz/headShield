#pragma once
#include "movingAverage.h"

class infraredSensor : public MovingAverage
{
public:
    int pin;
    bool state;
    bool prevState;
    MovingAverage headSensorAverageAnalog;

    infraredSensor(int pin)
    {
        this->pin = pin;
    }
    int read()
    {
        return analogRead(pin);
    }
    int readAverage()
    {
        headSensorAverageAnalog.add(read());
        return headSensorAverageAnalog.average();
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
