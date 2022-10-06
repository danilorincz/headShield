#pragma once

class powerLED
{
public:
    int percent;
    int ledPin;
    int level;
    powerLED(int ledPin)
    {
        this->ledPin = ledPin;
    }

    void setPercent(int percent)
    {
        int valuePWM = map(percent, 0, 100, 0, 255);
        analogWrite(ledPin, valuePWM);
    }
    void increaseLevel()
    {
        if (level < 3)
            level++;
        else
            level = 0;
    }
    void setLevel(int level)
    {
        switch (level)
        {
        case 0:
            setPercent(0);
            break;
        case 1:
            setPercent(33);
            break;
        case 2:
            setPercent(66);
            break;
        case 3:
            setPercent(100);
            break;

        }
    }
};
