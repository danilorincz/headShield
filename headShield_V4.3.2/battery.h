#pragma once

class Battery
{
public:
    int pin;
    unsigned int charge;
    int level;
    int threshold_min = 3400;
    int threshold_max = 3800;

    Battery(int pin)
    {
        this->pin = pin;
    }
    void begin()
    {
        pinMode(pin, INPUT);
    }
    int getCharge()
    {
        int batChargeRaw = getRaw();
        charge = map(batChargeRaw, 2925, 4096, 0, 100);
        return charge;
    }
    int getLevel()
    {
        getCharge();
        if (charge < threshold_min)
            level = 1;
        else if (charge < threshold_max)
            level = 2;
        else
            level = 3;
        return level;
    }
    int getRaw()
    {
        return analogRead(pin);
    }
};
