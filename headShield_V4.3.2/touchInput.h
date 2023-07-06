#pragma once

class Touch
{
private:
    int pin;
    int threshold;

private: //* SINGLE TAP
    int singleTapTimeMax = 400;
    int singleTapTimeMin = 30;
    bool released;
    long int singleTouchStartTime;
    bool singleTouchInProgress = false;

private: //* LONG TAP
    bool longTouchInProgress = false;
    unsigned long longTouchStartTime = 0;
    unsigned long longTapTreshold = 1000;

private: //* DOUBLE TAP
    int doubleTapTimeMin = 30;
    int doubleTapTimeMax = 250;
    int sequence = 0;

    long int firstTapStart;
    long int firstTapEnd;
    long int secondTapStart;
    long int secondTapEnd;

    int firstTapDuration;
    int betweenTapsDuration;
    int secondTapDuration;

public:
    Touch(int pin, int threshold) : pin(pin), threshold(threshold) {}

    int getAnalog()
    {
        touchRead(pin);
        return touchRead(pin);
    }

    bool getDigital()
    {
        int value = getAnalog();
        if (value < threshold)
        {
            return true;
        }
        else
            return false;
    }
    bool singleTap()
    {
        if (getDigital())
        {
            if (!singleTouchInProgress)
            {
                singleTouchStartTime = millis();
                singleTouchInProgress = true;
            }
        }
        else if (singleTouchInProgress)
        {
            if ((millis() - singleTouchStartTime) > singleTapTimeMin && (millis() - singleTouchStartTime) < singleTapTimeMax)
            {
                singleTouchInProgress = false;
                return true;
            }
        }
        else
        {
            singleTouchInProgress = false;
            singleTouchStartTime = 0;
        }
        return false;
    }

    bool longTap()
    {
        if (!getDigital())
        {
            longTouchInProgress = false;
            longTouchStartTime = 0;
            return false;
        }
        else
        {
            if (!longTouchInProgress)
            {
                longTouchStartTime = millis();
                longTouchInProgress = true;
            }
            if ((millis() - longTouchStartTime) > longTapTreshold && longTouchInProgress)
            {
                longTouchInProgress = false;
                return true;
            }
        }
        return false;
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
