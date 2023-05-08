#pragma once

class Tachometer
{
public:
    int pin;
    static bool interrupted;
    int speed_rpm = 0;
    int level;

    Tachometer(int pin)
    {
        this->pin = pin;
    }
    void begin()
    {
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), ISR, FALLING);
    }

    static void ISR()
    {
        interrupted = true;
    }

    int getRPM()
    {
        speed_rpm = speed_rpm;
        return speed_rpm;
    }
};

bool Tachometer::interrupted; // Removed initialization from here
