#pragma once

struct MajorityResult
{
  bool isMajority;
  int majorityValue;
};

MajorityResult getMajority(int index)
{
  // Static variables to maintain state between function calls
  static int buffer[5] = {-1, -1, -1, -1, -1};
  static int bufferIndex = 0;

  MajorityResult result;
  result.isMajority = false;
  result.majorityValue = -1;

  // Update the buffer with the new index
  buffer[bufferIndex] = index;
  bufferIndex = (bufferIndex + 1) % 5;

  // Check if the last five indices are the same
  int firstValue = buffer[0];
  for (int i = 1; i < 5; ++i)
  {
    if (buffer[i] != firstValue)
    {
      return result;
    }
  }

  result.isMajority = true;
  result.majorityValue = firstValue;
  return result;
}