#pragma once

class Tachometer
{
public:
    int pin;
    bool interrupted = false;
    int
    Tachometer(int pin)
    {
        this->pin = pin;
    }
    void begin()
    {
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), ISR, FALLING);
    }

    void ISR()
    {
        interrupted = true;
    }

    int getRPM()
    {
        //calaculate
        RPM = RPM;
        return RPM;
    }
};