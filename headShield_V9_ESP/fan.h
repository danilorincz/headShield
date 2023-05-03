#pragma once

class Fan
{
public:
    int PWMPin;
    int lastPercent;
    int level = 0;
    int prevLevel = 0;

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

    int increaseLevel()
    {
        if (level < 3)
            level++;
        else
            level = 0;
        return level;
    }
    void setLevel(int _level)
    {
        switch (_level)
        {
        case 0:
            setSpeed(0);
            break;
        case 1:
            setSpeed(80);
            break;
        case 2:
            setSpeed(90);
            break;
        case 3:
            setSpeed(100);
            break;
        }
    }
    void toggle()
    {
        increaseLevel();
        setLevel(level);
    }
};
