#pragma once

class Device
{
public:
    int pin;
    bool state;
    unsigned long timeWhenOn = 0;

    Device(int pin) : pin(pin) {}

    void begin()
    {
        pinMode(pin, OUTPUT);
    }
    void turnOn()
    {
        digitalWrite(pin, HIGH);
        timeWhenOn = millis();
        state = HIGH;
    }
    void turnOff()
    {
        digitalWrite(pin, LOW);
        timeWhenOn = 0;
        state = LOW;
    }
    unsigned long getCurrentSessionOn()
    {
        if (timeWhenOn == 0)
            return 0;
        else
            return millis() - timeWhenOn;
    }
    virtual bool on()
    {
        if (!state)
        {
            turnOn();
            return true;
        }
        return false;
    }
    virtual bool off()
    {
        if (state)
        {
            turnOff();
            return true;
        }
        return false;
    }
    virtual bool toggle()
    {
        if (state)
            turnOff();
        else
            turnOn();
        return state;
    }
    bool active()
    {
        return state;
    }
    void suspend()
    {
        digitalWrite(pin, LOW);
        timeWhenOn = -1;
    }
};
