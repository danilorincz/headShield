#pragma once

#include <Preferences.h>
#include "stat_data_struct.h"

void clearSerialBuffer()
{
  for (int i = 0; i < 64; i++)
  {
    int a = Serial.read();
  }
}

String restoreID()
{
  data.begin("data", false);
  delay(10);
  String ID = data.getString("ID", deviceID);
  delay(10);
  data.end();
  return ID;
}
bool setID(String &inputCommand)
{
  if (inputCommand.startsWith("set ID: "))
  {
    String newID = inputCommand.substring(8);
    data.begin("data", false);
    delay(10);
    data.putString("ID", newID);
    delay(10);
    data.end();
    Serial.print("New ID set to: " + newID);
    inputCommand = "";
    return true;
  }
  return false;
}

String getID()
{
  data.begin("data", false);
  delay(10);
  String myID = data.getString("ID", deviceID);
  delay(10);
  data.end();
  return myID;
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

unsigned long minutes_to_millis(unsigned long minutes)
{
  return minutes * 60 * 1000;
}

String millisToTimeString(unsigned long millis)
{
  // Calculate hours, minutes, and seconds
  unsigned long totalSeconds = millis / 1000;
  unsigned long hours = totalSeconds / 3600;
  unsigned long remainingSeconds = totalSeconds % 3600;
  unsigned long minutes = remainingSeconds / 60;
  unsigned long seconds = remainingSeconds % 60;

  // Convert to String and format
  String hoursStr = String(hours);
  String minutesStr = String(minutes);
  String secondsStr = String(seconds);

  // Add leading zeros if needed
  if (hours < 10)
  {
    hoursStr = "0" + hoursStr;
  }
  if (minutes < 10)
  {
    minutesStr = "0" + minutesStr;
  }
  if (seconds < 10)
  {
    secondsStr = "0" + secondsStr;
  }

  String timeString = hoursStr + ":" + minutesStr + ":" + secondsStr;

  return timeString;
}
