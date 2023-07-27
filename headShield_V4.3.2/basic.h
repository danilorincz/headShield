#pragma once
#include <Preferences.h>
#include "stat_data_struct.h"

void restore(FanCondition &intoThis, Preferences &fromHere, String mapName)
{
  StatData loaded;
  fromHere.begin(mapName.c_str(), false);
  loaded.max = fromHere.getInt("max", -1);
  loaded.min = fromHere.getInt("min", -1);
  loaded.average = fromHere.getInt("ave", -1);
  fromHere.end();

  intoThis.setLimit(loaded);
}

void putData(StatData putThis, Preferences &here, String mapName)
{
  here.begin(mapName.c_str(), false);
  here.putInt("max", putThis.max);
  here.putInt("min", putThis.min);
  here.putInt("ave", putThis.average);
  here.end();
}
void serialPrintIf(String inputText)
{
  if (serialEnabled)
    Serial.println(inputText);
}
bool intersect(StatData range_1, StatData range_2)
{

  if (range_1.max < range_2.min || range_2.max < range_1.min)
  {
    return false;
  }
  else
  {
    return true;
  }
}
