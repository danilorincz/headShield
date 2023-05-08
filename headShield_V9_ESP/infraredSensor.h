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
        if (analogRead(pin) < 3000)
            state = true;
        else
            state = false;
            
        return state;
    }
};
