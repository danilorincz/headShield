#pragma once

class FanCondition
{
public:
  int upperLimit;
  int lowerLimit;

  FanCondition(int lowerLimit, int upperLimit)
  {
  }
  void setLimit(int lowerLimit, int upperLimit)
  {
    this->upperLimit = upperLimit;
    this->lowerLimit = lowerLimit;
  }
  int getMax()
  {
    return this->upperLimit;
  }
  int getMin()
  {
    return this->lowerLimit;
  }
  bool inRange(int value)
  {
    if (lowerLimit <= value && value <= upperLimit)
      return true;
    return false;
  }
};