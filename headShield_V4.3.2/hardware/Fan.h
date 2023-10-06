#pragma once
#include "Device.h"
#include "..\stat_data_struct.h"

class Fan : public Device
{
public:
    Fan(int pin) : Device(pin) {}

};

class FanCondition
{
public:
    StatData limits;

    FanCondition()
    {
    }
    void setLimit(StatData setData)
    {
        this->limits.max = setData.max;
        this->limits.min = setData.min;
        this->limits.average = setData.average;
    }
    void setMax(int newMax)
    {
        this->limits.max = newMax;
    }
    void setMin(int newMin)
    {
        this->limits.min = newMin;
    }
    int getMax()
    {
        return limits.max;
    }
    int getMin()
    {
        return limits.min;
    }
    int getAverage()
    {
        return limits.average;
    }
    void clear()
    {
        limits.min = -1;
        limits.max = -1;
        limits.average = -1;
    }
    StatData getStatData()
    {
        return limits;
    }
};

namespace cond
{
    enum conditionNumber
    {
        off,
        underNormal,
        normal,
        overNormal,
        other
    };
}