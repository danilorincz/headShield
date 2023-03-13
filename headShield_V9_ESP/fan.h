#pragma once

class Fan
{
public:
    int PWMPin;
    int lastPercent;
    int level = 0;

    Fan(int PWMPin)
    {
        this->PWMPin = PWMPin;
    }
    void begin()
    {
        pinMode(PWMPin, OUTPUT);
    }

    void setSpeed(int percent)
    {
        if (percent != lastPercent)
        {
            int PWMvalue = map(percent, 0, 100, 0, 255);
            analogWrite(PWMPin, PWMvalue);
            lastPercent = percent;
        }
    }
    void on()
    {
        digitalWrite(PWMPin, HIGH);
    }
    void off()
    {
        digitalWrite(PWMPin, LOW);
    }
    void increaseLevel()
    {
        level++;
        if (level == 3)
            level = 0;
    }
    void setLevel(int level_)
    {
        switch (level_)
        {
        case 0:
            setSpeed(0);
            break;
        case 1:
            setSpeed(60);
            break;
        case 2:
            setSpeed(100);
            break;
        }
    }
};
