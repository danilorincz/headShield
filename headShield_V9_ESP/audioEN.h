#pragma once

class Audio
{
public:
    int enablePin;
    bool state;
    bool prevState;
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
    bool toggle()
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
        return state;
    }
};
