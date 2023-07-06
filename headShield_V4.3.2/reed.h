#pragma once

class ReedSwitch
{
public:
    int pin;
    bool state;
    ReedSwitch(int pin)
    {
        this->pin = pin;
    }
    void begin()
    {
        pinMode(pin, INPUT_PULLUP);
    }
    bool scan()
    {
        if (digitalRead(pin))
            state = true;
        else
            state = false;
        return state;
    }
};