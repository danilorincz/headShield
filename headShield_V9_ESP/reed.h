#pragma once

class ReedSwitch
{
public:
    int pin;
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
            return true;
        else
            return false;
    }
};