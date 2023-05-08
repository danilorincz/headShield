#pragma once

class Fan
{
private:
    int percent;

public:
    int PWMPin;
    int level = 0;

    Fan(int PWMPin)
    {
        this->PWMPin = PWMPin;
    }
    void begin()
    {
        pinMode(PWMPin, OUTPUT);
    }

    void setSpeed(int _percent)
    {
        if (_percent != this->percent)
        {
            int PWMvalue = map(_percent, 0, 100, 0, 255);
            analogWrite(PWMPin, PWMvalue);
            percent = _percent;
        }
    }
    void on()
    {
        digitalWrite(PWMPin, HIGH);
        level = 3;
    }
    void off()
    {
        digitalWrite(PWMPin, LOW);
        level = 0;
    }

    int increaseLevel(int min, int max)
    {
        if (level < max)
            level++;
        else
            level = min;
        return level;
    }
    void setLevel(int _level)
    {
        level = _level;

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
    void temporaryOff()
    {
        analogWrite(PWMPin, 0);
        percent = 0;
    }
    void toggle(int min, int max)
    {
        increaseLevel(min, max);
        setLevel(level);
    }
};
