#pragma once

class infraredSensor
{
public:
    int pin;
    bool state;
    bool prevState;
    infraredSensor(int pin)
    {
        this->pin = pin;
    }
    int read()
    {
        return analogRead(pin);
    }
    bool scan()
    {
        if (read() != 4095)
            state = true;
        else
            state = false;

        return state;
    }
};
