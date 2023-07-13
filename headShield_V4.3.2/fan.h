#pragma once
#include "device.h"

class Fan : public Device
{
public:
    Fan(int pin) : Device(pin) {}

    virtual void setLevel(int _level) override
    {
        level = _level;

        switch (_level)
        {
        case 0:
            setIntensity(0);
            break;
        case 1:
            setIntensity(60);
            break;
        case 2:
            setIntensity(85);
            break;
        case 3:
            setIntensity(100);
            break;
        default:
            Serial.println("SWITCH ERROR");
        }
    }

};
