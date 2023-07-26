#pragma once

class Device
{
public:
    int pin;
    bool state;

    Device(int pin) : pin(pin) {}

    void begin()
    {
        pinMode(pin, OUTPUT);
    }

    virtual bool on()
    {
        if (!state)
        {
            state = HIGH;
            digitalWrite(pin, state);
            return true;
        }
        return false;
    }
    virtual bool off()
    {
        if (state)
        {
            state = LOW;
            digitalWrite(pin, state);
            return true;
        }
        return false;
    }
    virtual bool toggle()
    {
        state = !state;
        digitalWrite(pin, state);
        return state;
    }
    bool active()
    {
        return state;
    }
    void suspend()
    {
        digitalWrite(pin, LOW);
    }
};
