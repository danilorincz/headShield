#pragma once

class FanCondition
{
public:
  int upperLimit;
  int lowerLimit;

  FanCondition(int lowerLimit, int upperLimit)
  {
    this->upperLimit = upperLimit;
    this->lowerLimit = lowerLimit;
  }
  
  bool inRange(int value)
  {
    if (lowerLimit <= value && value <= upperLimit)
      return true;
    return false;
  }
};