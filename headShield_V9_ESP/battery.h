#pragma once

class Battery
{
public:
    int pin;
    unsigned int chargePercent;
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
        chargePercent = map(batChargeRaw, 2925, 4096, 0, 100);
        return chargePercent;
    }
    int getRaw()
    {
        return analogRead(pin);
    }
};
