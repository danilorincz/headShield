#pragma once

class FanCondition
{
public:
  int max;
  int min;

  FanCondition()
  {
  }
  void setLimit(int min, int max)
  {
    this->max = max;
    this->min = min;
  }
  int getMax()
  {
    return this->max;
  }
  int getMin()
  {
    return this->min;
  }
  bool inRange(int value)
  {
    if (min <= value && value <= max)
      return true;
    return false;
  }
};