#pragma once
#include "device.h"

class LED : public Device
{
public:
    LED(int pin) : Device(pin) {}

    virtual void setLevel(int _level) override
    {
        level = _level;

        switch (_level)
        {
        case 0:
            setIntensity(0);
            break;
        case 1:
            setIntensity(33);
            break;
        case 2:
            setIntensity(66);
            break;
        case 3:
            setIntensity(100);
            break;
        default:
            Serial.println("SWITCH ERROR");
        }
    }
};
