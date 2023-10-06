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

String restoreWifiCredentials()
{
  data.begin("data", false);
  String SSID = data.getString("SSID", default_ssid);
  data.end();
  return SSID;
}
bool setSSID(String &inputCommand)
{
  if (inputCommand.startsWith("setSSID: "))
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



String getSSID()
{
  data.begin("data", false);
  String mySSID = data.getString("SSID", default_ssid);
  data.end();
  return mySSID;
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

bool recoverDeviceSpecificData()
{
  data.begin("data", false);
  String loadedSSID = data.getString("SSID", default_ssid);
  data.end();

  if (loadedSSID == "HS_1")
  {
    battery.a = 1.23;
    battery.b = -1.35;
    ADAPT.ABSOLUTE_MIN = 3300;
    ADAPT.NOFILTER_CONST_MAX = 3530;
    ADAPT.NORMAL_INITIAL_MIN = 3535;
    ADAPT.NORMAL_CONST_MIN = 3545;
    ADAPT.NORMAL_CONST_MAX = 3585;
    ADAPT.NORMAL_INITIAL_MAX = 3600;
    ADAPT.NOAIR_CONST_MIN = 3605;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
    normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
    normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
  }
  else if (loadedSSID == "HS_2")
  {
    battery.a = 0.92;
    battery.b = 0.56;
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3595;
    ADAPT.NORMAL_INITIAL_MIN = 3600;
    ADAPT.NORMAL_CONST_MIN = 3610;
    ADAPT.NORMAL_CONST_MAX = 3665;
    ADAPT.NORMAL_INITIAL_MAX = 3675;
    ADAPT.NOAIR_CONST_MIN = 3680;
    ADAPT.ABSOLUTE_MAX = 3800;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
    normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
    normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
  }
  else if (loadedSSID == "HS_3")
  {
    battery.a = 1.02;
    battery.b = -0.11;
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3580;
    ADAPT.NORMAL_INITIAL_MIN = 3570;
    ADAPT.NORMAL_CONST_MIN = 3590;
    ADAPT.NORMAL_CONST_MAX = 3630;
    ADAPT.NORMAL_INITIAL_MAX = 3640;
    ADAPT.NOAIR_CONST_MIN = 3645;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
    normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
    normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
  }
  else if (loadedSSID == "HS_4")
  {
    battery.a = 0.94;
    battery.b = 0.48;
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3575;
    ADAPT.NORMAL_INITIAL_MIN = 3570;
    ADAPT.NORMAL_CONST_MIN = 3580;
    ADAPT.NORMAL_CONST_MAX = 3630;
    ADAPT.NORMAL_INITIAL_MAX = 3640;
    ADAPT.NOAIR_CONST_MIN = 3645;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
    normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
    normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
  }
  else
  {
    Serial.println("SSID not found");
    return false;
  }
  Serial.println("Adaptive values loaded for" + loadedSSID);

  return true;
}
