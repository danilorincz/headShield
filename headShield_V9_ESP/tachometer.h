#pragma once

class Tachometer
{
public:
    int pin;
    int speed_rpm = 3;

    Tachometer(int pin)
    {
        this->pin = pin;
    }
    void begin()
    {
        pinMode(pin, INPUT);
    }
    int getAnalog()
    {
        return analogRead(pin);
    }
    int getSpeed()
    {
       return getAnalog(); 
    }
};
