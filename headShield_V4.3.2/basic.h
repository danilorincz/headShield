#pragma once
#include <Preferences.h>
#include "stat_data_struct.h"

void restoreCondition(FanCondition &intoThis, Preferences &fromHere, String mapName)
{
  StatData loaded;
  fromHere.begin(mapName.c_str(), false);
  delay(10);
  loaded.max = fromHere.getInt("max", -1);
  loaded.min = fromHere.getInt("min", -1);
  loaded.average = fromHere.getInt("ave", -1);
  fromHere.end();
  delay(10);
  intoThis.setLimit(loaded);
}
String restoreWifiCredentials(Preferences &fromHere)
{
  fromHere.begin("data", false);
  String SSID = fromHere.getString("SSID", default_ssid);
  fromHere.end();
  return SSID;
}
bool setSSID(String &inputCommand)
{
  if (inputCommand.startsWith("setSSID:"))
  {
    String newSSID = inputCommand.substring(8);
    data.begin("data", false);
    data.putString("SSID", newSSID);
    data.end();
    WiFi.softAP(newSSID.c_str(), password);
    Serial.print("New SSID set to: ");
    Serial.println(newSSID);
    inputCommand = "";
    return true;
  }
  return false;
}

void restoreBatteryCorrection()
{
  data.begin("batPar", false);
  delay(10);
  float a = data.getFloat("a", -1);
  float b = data.getFloat("b", -1);
  delay(10);
  data.end();
  battery.setCorrection(a, b);
}

String getSSID()
{
  data.begin("data", false);
  String mySSID = data.getString("SSID", default_ssid);
  data.end();
  return mySSID;
}

bool setNormal(String &inputCommand)
{
  if (inputCommand.startsWith("set min:"))
  {
    String newMinString = inputCommand.substring(8);
    int newMin = newMinString.toInt();
    data.begin("normal");
    normal.setMin(newMin);
    delay(10);
    data.putInt("min", newMin);
    delay(10);
    data.end();
    inputCommand = "";
    Serial.println("SET MIN");
    return true;
  }
  if (inputCommand.startsWith("set max:"))
  {
    String newMaxString = inputCommand.substring(8);
    int newMax = newMaxString.toInt();
    data.begin("normal");
    normal.setMax(newMax);
    delay(10);
    data.putInt("max", newMax);
    delay(10);
    data.end();
    inputCommand = "";
    Serial.println("SET MAX");
    return true;
  }

  return false;
}

void setBatteryParameter(String &inputCommand)
{
  float new_a = 0.0;
  float new_b = 0.0;

  if (!inputCommand.startsWith("set bat:"))
    return;

  inputCommand.remove(0, 8);
  inputCommand.trim();

  int spaceIndex = inputCommand.indexOf(' ');

  if (spaceIndex == -1)
    return;

  String stra = inputCommand.substring(0, spaceIndex);
  String strb = inputCommand.substring(spaceIndex + 1);

  new_a = stra.toFloat();
  new_b = strb.toFloat();

  Serial.print("new_a: ");
  Serial.println(new_a, 2);
  Serial.print("new_b: ");
  Serial.println(new_b, 2);

  data.begin("batPar");
  delay(10);
  data.putFloat("a", new_a);
  data.putFloat("b", new_b);
  data.end();

  battery.setCorrection(new_a, new_b);

  inputCommand = "";
}

void putData(StatData putThis, Preferences &here, String mapName)
{
  here.begin(mapName.c_str(), false);
  delay(10);
  here.putInt("max", putThis.max);
  here.putInt("min", putThis.min);
  here.putInt("ave", putThis.average);
  here.end();
  delay(10);
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

  // Combine into final time string
  String timeString = hoursStr + ":" + minutesStr + ":" + secondsStr;

  return timeString;
}
