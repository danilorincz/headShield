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
        for (int i = 0; i < 10; i++)
        {
            if (read() < 3600)
                return true;
        }
        return false;
    }


    
};
