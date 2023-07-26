#pragma once

#include "StatData.h"

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
  StatData getStatData()
  {
    return limits;
  }
  bool inRange(int value, int offset)
  {
 
    if (limits.min - offset <= value && value <= limits.max + offset)
      return true;
    return false;
  }
};
