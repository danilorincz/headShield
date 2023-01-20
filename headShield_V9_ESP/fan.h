#pragma once

class Fan
{
public:
    int PWMPin;
    int lastPercent;

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
};
