#pragma once

class Audio
{
public:
    int enablePin;
    Audio(int enablePin)
    {
        this->enablePin = enablePin;
    }
    void begin()
    {
        pinMode(enablePin, OUTPUT);
    }
    void on()
    {
        digitalWrite(enablePin, HIGH);
    }
    void off()
    {
        digitalWrite(enablePin, LOW);
    }
};
