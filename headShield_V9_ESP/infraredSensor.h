#pragma once

class infraredSensor
{
public:
    int pin;
    infraredSensor(int pin)
    {
        this->pin = pin;
    }
    int read()
    {
        return analogRead(pin);
    }
    bool active()
    {
        if (analogRead(pin) < 3000)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};