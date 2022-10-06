#pragma once

class Fan
{
public:
    int tachoPin;
    int PWMPin;

    unsigned long measuredRPM;
    unsigned int periodTime;
    unsigned int orederedPercent;

    Fan(int tachoPin, int PWMPin)
    {
        this->tachoPin = tachoPin;
        this->PWMPin = PWMPin;
    }
    void begin()
    {
        pinMode(tachoPin, INPUT);
        pinMode(PWMPin, OUTPUT);
    }

    void setSpeed(int percent)
    {
        orederedPercent = percent;
        int PWMvalue = map(percent, 0, 100, 0, 255);
        analogWrite(PWMPin, PWMvalue);
    }

    bool sensingReflection()
    {
        if (!digitalRead(tachoPin) && !digitalRead(tachoPin))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int getPeriodTime()
    {
        bool flag = true;
        int counter = 0;
        unsigned long preTime = 0;

        while (sensingReflection())
            ;

        preTime = millis();
         
        while (millis() - preTime < 300)
        {
            if (flag)
            {
                if (sensingReflection())
                {
                    counter++;
                    flag = false;
                }
            }
            else
            {
                if (!sensingReflection())
                {
                    flag = true;
                }
            }
        }

        return counter;
    }
};
