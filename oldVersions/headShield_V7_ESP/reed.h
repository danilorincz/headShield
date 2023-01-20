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
    bool read()
    {
        if (digitalRead(pin))
            return false;
        else
            return true;
    }
};