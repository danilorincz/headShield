#pragma once

class Audio
{
public:
    int enablePin;
    bool state;
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
        if (!state)
        {
            digitalWrite(enablePin, HIGH);
            state = HIGH;
        }
    }
    void off()
    {
        if (state)
        {
            digitalWrite(enablePin, LOW);
            state = LOW;
        }
    }
    void toggle()
    {
        if (state)
        {
            digitalWrite(enablePin, LOW);
            state = LOW;
        }
        else
        {
            digitalWrite(enablePin, HIGH);
            state = HIGH;
        }
    }
};
