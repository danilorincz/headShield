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
  String newValueString = inputCommand.substring(8);
  int newValue = newValueString.toInt();
  data.begin("normal");
  delay(10);
  bool returnValue = false;
  if (inputCommand.startsWith("set min:"))
  {
    normal.setMin(newValue);
    data.putInt("min", newValue);
    Serial.println("SET MIN");
    returnValue = true;
  }
  if (inputCommand.startsWith("set max:"))
  {
    normal.setMax(newValue);
    data.putInt("max", newValue);
    Serial.println("SET MAX");
    returnValue = true;
  }
  if (returnValue)
  {
    delay(10);
    data.end();
    inputCommand = "";
  }
  return returnValue;
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

void saveAdaptiveSettings(Preferences &prefs, const AdaptiveValues &settings)
{
  prefs.begin("settings", false);
  delay(10);
  prefs.putUInt("ABS_MIN", settings.ABSOLUTE_MIN);
  prefs.putUInt("NOF_MAX", settings.NOFILTER_CONST_MAX);
  prefs.putUInt("NOR_INI_MIN", settings.NORMAL_INITIAL_MIN);
  prefs.putUInt("NOR_CON_MIN", settings.NORMAL_CONST_MIN);
  prefs.putUInt("NOR_CON_MAX", settings.NORMAL_CONST_MAX);
  prefs.putUInt("NOR_INI_MAX", settings.NORMAL_INITIAL_MAX);
  prefs.putUInt("NOA_MIN", settings.NOAIR_CONST_MIN);
  prefs.putUInt("ABS_MAX", settings.ABSOLUTE_MAX);
  prefs.putUInt("LOW_DIF", settings.LOWER_DIF);
  prefs.putUInt("UPP_DIF", settings.UPPER_DIF);
  prefs.putFloat("MAX_ACC", settings.MAX_ACCEL);
  prefs.end();
  delay(10);
}

void loadAdaptiveSettings(Preferences &prefs, AdaptiveValues &settings)
{
  prefs.begin("settings", false);
  delay(10);
  settings.ABSOLUTE_MIN = prefs.getUInt("ABS_MIN", 3400);
  settings.NOFILTER_CONST_MAX = prefs.getUInt("NOF_MAX", 3555);
  settings.NORMAL_INITIAL_MIN = prefs.getUInt("NOR_INI_MIN", 3570);
  settings.NORMAL_CONST_MIN = prefs.getUInt("NOR_CON_MIN", 3575);
  settings.NORMAL_CONST_MAX = prefs.getUInt("NOR_CON_MAX", 3620);
  settings.NORMAL_INITIAL_MAX = prefs.getUInt("NOR_INI_MAX", 3640);
  settings.NOAIR_CONST_MIN = prefs.getUInt("NOA_MIN", 3645);
  settings.ABSOLUTE_MAX = prefs.getUInt("ABS_MAX", 3700);
  settings.LOWER_DIF = prefs.getUInt("LOW_DIF", 25);
  settings.UPPER_DIF = prefs.getUInt("UPP_DIF", 15);
  settings.MAX_ACCEL = prefs.getFloat("MAX_ACC", 0.65);
  prefs.end();
  delay(10);
}

bool setAdaptiveSettings(String &inputCommand, AdaptiveValues &settings)
{
  bool returnValue = true;

  if (inputCommand.startsWith("set absMin: "))
  {
    settings.ABSOLUTE_MIN = inputCommand.substring(11).toInt();
    Serial.print("New ABSOLUTE_MIN set to: ");
    Serial.println(settings.ABSOLUTE_MIN);
  }
  else if (inputCommand.startsWith("set noFilterMax: "))
  {
    settings.NOFILTER_CONST_MAX = inputCommand.substring(16).toInt();
    Serial.print("New NOFILTER_CONST_MAX set to: ");
    Serial.println(settings.NOFILTER_CONST_MAX);
  }
  else if (inputCommand.startsWith("set normalInitMin: "))
  {
    settings.NORMAL_INITIAL_MIN = inputCommand.substring(18).toInt();
    Serial.print("New NORMAL_INITIAL_MIN set to: ");
    Serial.println(settings.NORMAL_INITIAL_MIN);
  }
  else if (inputCommand.startsWith("set normalConstMin: "))
  {
    settings.NORMAL_CONST_MIN = inputCommand.substring(18).toInt();
    Serial.print("New NORMAL_CONST_MIN set to: ");
    Serial.println(settings.NORMAL_CONST_MIN);
  }
  else if (inputCommand.startsWith("set normalConstMax: "))
  {
    settings.NORMAL_CONST_MAX = inputCommand.substring(18).toInt();
    Serial.print("New NORMAL_CONST_MAX set to: ");
    Serial.println(settings.NORMAL_CONST_MAX);
  }
  else if (inputCommand.startsWith("set normalInitMax: "))
  {
    settings.NORMAL_INITIAL_MAX = inputCommand.substring(18).toInt();
    Serial.print("New NORMAL_INITIAL_MAX set to: ");
    Serial.println(settings.NORMAL_INITIAL_MAX);
  }
  else if (inputCommand.startsWith("set noAirMin: "))
  {
    settings.NOAIR_CONST_MIN = inputCommand.substring(13).toInt();
    Serial.print("New NOAIR_CONST_MIN set to: ");
    Serial.println(settings.NOAIR_CONST_MIN);
  }
  else if (inputCommand.startsWith("set absMax: "))
  {
    settings.ABSOLUTE_MAX = inputCommand.substring(11).toInt();
    Serial.print("New ABSOLUTE_MAX set to: ");
    Serial.println(settings.ABSOLUTE_MAX);
  }
  else if (inputCommand.startsWith("set lowerDif: "))
  {
    settings.LOWER_DIF = inputCommand.substring(13).toInt();
    Serial.print("New LOWER_DIF set to: ");
    Serial.println(settings.LOWER_DIF);
  }
  else if (inputCommand.startsWith("set upperDif: "))
  {
    settings.UPPER_DIF = inputCommand.substring(13).toInt();
    Serial.print("New UPPER_DIF set to: ");
    Serial.println(settings.UPPER_DIF);
  }
  else if (inputCommand.startsWith("set maxAccel: "))
  {
    settings.MAX_ACCEL = inputCommand.substring(13).toFloat();
    Serial.print("New MAX_ACCEL set to: ");
    Serial.println(settings.MAX_ACCEL, 2); // 2 decimal places
  }
  else
  {
    returnValue = false; // No matching command
  }

  // Save the updated settings
  if (returnValue)
  {
    saveAdaptiveSettings(data, settings);
    inputCommand = ""; // Clear the command
  }
  return returnValue; // Successfully set a setting
}
