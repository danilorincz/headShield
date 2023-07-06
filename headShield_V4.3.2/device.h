#pragma once

class Device
{
public:
    int pin;
    int percent;
    int level;

    Device(int pin) : pin(pin), level(0), percent(0) {}

    void begin()
    {
        pinMode(pin, OUTPUT);
    }

    virtual void setLevel(int _level) = 0; // Pure virtual function for setting the level

    int increaseLevel(int min, int max)
    {
        if (level < max)
            level++;
        else
            level = min;
        return level;
    }
    void setIntensity(int _percent)
    {
        if (_percent != this->percent)
        {
            int value = map(_percent, 0, 100, 0, 255);
            analogWrite(pin, value);
            percent = _percent;
        }
    }
    void toggle(int min, int max)
    {
        increaseLevel(min, max);
        setLevel(level);
    }
    void suspend()
    {
        analogWrite(pin, 0);
    }
};
