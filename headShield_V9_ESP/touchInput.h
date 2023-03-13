#pragma once

class touchInput
{
public:
    int pin;
    int threshold;

    //? SINGLE TAP
    int singleTapTime = 100;
    long int timeWhenSingleTap = 0;
    bool timeStartedFlag = false;
    bool releasedFlag = false;

    //? DOUBLE TAP
    int doubleTapTimeMin = 30;
    int doubleTapTimeMax = 250;
    int sequence = 0;
    long int t0 = 0;
    long int t1 = 0;
    long int t2 = 0;
    long int t3 = 0;
    int T01 = 0;
    int T12 = 0;
    int T23 = 0;

    touchInput(int pin, int threshold)
    {
        this->pin = pin;
        this->threshold = threshold;
    }
    int readRaw()
    {
        return touchRead(pin);
    }
    bool readAtTheMoment()
    {
        if (touchRead(pin) < threshold)
            return true;
        else
            return false;
    }

    bool singleTap()
    {
        if (readAtTheMoment())
        {
            if (!timeStartedFlag)
            {
                timeWhenSingleTap = millis();
                timeStartedFlag = true;
            }
            if (millis() - timeWhenSingleTap > singleTapTime && releasedFlag)
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
        if (readAtTheMoment())
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
            if (readAtTheMoment())
            {
                sequence = 1;
                t0 = millis();
            }
            break;
        case 1:
            if (!readAtTheMoment())
            {
                t1 = millis();
                T01 = t1 - t0;
                if (doubleTapTimeMin < T01 && T01 < doubleTapTimeMax)
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
            if (readAtTheMoment())
            {
                t2 = millis();
                T12 = t2 - t1;
                if (doubleTapTimeMin < T12 && T12 < doubleTapTimeMax)
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
            if (!readAtTheMoment())
            {
                t3 = millis();
                T23 = t3 - t2;
                if (doubleTapTimeMin < T23 && T23 < doubleTapTimeMax)
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
