#pragma once

class Audio
{
public:
    int enablePin;
    bool state;
    bool stateMemory;
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
    void temporaryOff()
    {
        digitalWrite(enablePin, LOW);
    }
    void turn(bool _state)
    {
        if (_state)
            state = HIGH;
        else
            state = LOW;
            
        digitalWrite(enablePin, state);
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
