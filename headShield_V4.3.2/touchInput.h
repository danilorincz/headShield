#pragma once

class touchInput
{
public:
    int pin;
    int threshold;

    //? SINGLE TAP
    int singleTapTimeMax = 100;
    int singleTapTimeMin = 30;
    long int timeWhenSingleTap = 0;
    bool timeStartedFlag = false;
    bool releasedFlag = false;

    //? DOUBLE TAP
    int doubleTapTimeMin = 30;
    int doubleTapTimeMax = 250;
    int sequence = 0;

    long int firstTapStart = 0;
    long int firstTapEnd = 0;
    long int secondTapStart = 0;
    long int secondTapEnd = 0;
    int firstTapDuration = 0;
    int betweenTapsDuration = 0;
    int secondTapDuration = 0;

    touchInput(int pin, int threshold)
    {
        this->pin = pin;
        this->threshold = threshold;
    }
    int getAnalog()
    {
        return touchRead(pin);
    }
    bool getDigital()
    {
        if (getAnalog() < threshold)
            return true;
        else
            return false;
    }

    bool singleTap()
    {
        if (getDigital())
        {
            if (!timeStartedFlag)
            {
                timeWhenSingleTap = millis();
                timeStartedFlag = true;
            }
            if (millis() - timeWhenSingleTap > singleTapTimeMax && releasedFlag)
            {
                releasedFlag = false;
                timeStartedFlag = false;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            releasedFlag = true;
            timeStartedFlag = false;
            return false;
        }
    }

    bool longTapTimeFlag = false;
    long int timeWhenLongTap = 0;
    int longTapTime = 1000;
    bool releasedLongTapFlag = false;

    bool longTap()
    {
        if (getDigital())
        {
            if (!longTapTimeFlag)
            {
                timeWhenLongTap = millis();
                longTapTimeFlag = true;
            }
            if (millis() - timeWhenLongTap > longTapTime && releasedLongTapFlag)
            {
                releasedLongTapFlag = false;
                longTapTimeFlag = false;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            releasedLongTapFlag = true;
            longTapTimeFlag = false;
            return false;
        }
    }

    bool doubleTap()
    {
        switch (sequence)
        {
        case 0:
            if (getDigital())
            {
                sequence = 1;
                firstTapStart = millis();
            }
            break;
        case 1:
            if (!getDigital())
            {
                firstTapEnd = millis();
                firstTapDuration = firstTapEnd - firstTapStart;
                if (doubleTapTimeMin < firstTapDuration && firstTapDuration < doubleTapTimeMax)
                {
                    sequence = 2;
                }
                else
                {
                    sequence = 0;
                }
            }
            break;
        case 2:
            if (getDigital())
            {
                secondTapStart = millis();
                betweenTapsDuration = secondTapStart - firstTapEnd;
                if (doubleTapTimeMin < betweenTapsDuration && betweenTapsDuration < doubleTapTimeMax)
                {
                    sequence = 3;
                }
                else
                {
                    sequence = 0;
                }
            }
            break;
        case 3:
            if (!getDigital())
            {
                secondTapEnd = millis();
                secondTapDuration = secondTapEnd - secondTapStart;
                if (doubleTapTimeMin < secondTapDuration && secondTapDuration < doubleTapTimeMax)
                {
                    sequence = 4;
                }
                else
                {
                    sequence = 0;
                }
            }
            break;
        case 4:
            sequence = 0;
            return true;
            break;
        }
        return false;
    }
};
