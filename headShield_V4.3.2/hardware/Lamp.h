#pragma once
#include "Device.h"

class LED : public Device
{
private:
    void setIntensity(int percent)
    {
        int value = map(percent, 0, 100, 0, 255);
        analogWrite(pin, value);
    }

public:
    int level;
    LED(int pin) : Device(pin) {}
    bool on() override
    {
        if (Device::on())
        {
            level = 3;
            return true;
        }
        return false;
    }
    bool off() override
    {
        if (Device::off())
        {
            level = 0;
            return true;
        }
        return false;
    }
    bool toggle() override
    {
        if (Device::toggle())
            level = 3;
        else
            level = 0;
        return state;
    }

    void setLevel(int newLevel)
    {

        switch (newLevel)
        {
        case 0:
            setIntensity(0);
            state = false;
            break;
        case 1:
            setIntensity(33);
            state = true;
            break;
        case 2:
            setIntensity(66);
            state = true;
            break;
        case 3:
            setIntensity(100);
            state = true;
            break;
        default:
            Serial.println("Level not in range");
            return;
        }

        level = newLevel;
    }

    void toggleBetween(int min, int max)
    {
        if (level < max)
            level++;
        else
            level = min;
        setLevel(level);
    }
};
