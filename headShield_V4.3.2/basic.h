#pragma once
#include <Preferences.h>
#include "stat_data_struct.h"

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

void saveAdaptiveSettings(AdaptiveValues &settings)
{
  data.begin("settings", false);
  delay(10);
  data.putUInt("ABS_MIN", settings.ABSOLUTE_MIN);
  data.putUInt("NOF_MAX", settings.NOFILTER_CONST_MAX);
  data.putUInt("NOR_INI_MIN", settings.NORMAL_INITIAL_MIN);
  data.putUInt("NOR_CON_MIN", settings.NORMAL_CONST_MIN);
  data.putUInt("NOR_CON_MAX", settings.NORMAL_CONST_MAX);
  data.putUInt("NOR_INI_MAX", settings.NORMAL_INITIAL_MAX);
  data.putUInt("NOA_MIN", settings.NOAIR_CONST_MIN);
  data.putUInt("ABS_MAX", settings.ABSOLUTE_MAX);
  data.putUInt("LOW_DIF", settings.LOWER_DIF);
  data.putUInt("UPP_DIF", settings.UPPER_DIF);
  data.putFloat("MAX_ACC", settings.MAX_ACCEL);
  data.end();
  delay(10);
}

void loadAdaptiveSettings(AdaptiveValues &settings)
{
  data.begin("settings", false);
  delay(10);
  settings.ABSOLUTE_MIN = data.getUInt("ABS_MIN", 0);
  settings.NOFILTER_CONST_MAX = data.getUInt("NOF_MAX", 0);
  settings.NORMAL_INITIAL_MIN = data.getUInt("NOR_INI_MIN", 0);
  settings.NORMAL_CONST_MIN = data.getUInt("NOR_CON_MIN", 0);
  settings.NORMAL_CONST_MAX = data.getUInt("NOR_CON_MAX", 0);
  settings.NORMAL_INITIAL_MAX = data.getUInt("NOR_INI_MAX", 0);
  settings.NOAIR_CONST_MIN = data.getUInt("NOA_MIN", 0);
  settings.ABSOLUTE_MAX = data.getUInt("ABS_MAX", 0);
  settings.LOWER_DIF = data.getUInt("LOW_DIF", 0);
  settings.UPPER_DIF = data.getUInt("UPP_DIF", 0);
  settings.MAX_ACCEL = data.getFloat("MAX_ACC", 0.65);
  data.end();
  delay(10);
}

bool setAdaptiveFromSSID()
{
  data.begin("data", false);
  String loadedSSID = data.getString("SSID", default_ssid);
  data.end();

  if (loadedSSID == "HS_1")
  {
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3520;
    ADAPT.NORMAL_INITIAL_MIN = 3525;
    ADAPT.NORMAL_CONST_MIN = 3535;
    ADAPT.NORMAL_CONST_MAX = 3580;
    ADAPT.NORMAL_INITIAL_MAX = 3595;
    ADAPT.NOAIR_CONST_MIN = 3600;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
  }
  else if (loadedSSID == "HS_2")
  {
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 0;
    ADAPT.NORMAL_INITIAL_MIN = 0;
    ADAPT.NORMAL_CONST_MIN = 0;
    ADAPT.NORMAL_CONST_MAX = 0;
    ADAPT.NORMAL_INITIAL_MAX = 0;
    ADAPT.NOAIR_CONST_MIN = 0;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
  }
  else if (loadedSSID == "HS_3")
  {
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3555;
    ADAPT.NORMAL_INITIAL_MIN = 3570;
    ADAPT.NORMAL_CONST_MIN = 3575;
    ADAPT.NORMAL_CONST_MAX = 3620;
    ADAPT.NORMAL_INITIAL_MAX = 3640;
    ADAPT.NOAIR_CONST_MIN = 3645;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
  }
  else if (loadedSSID == "HS_4")
  {
    ADAPT.ABSOLUTE_MIN = 3400;
    ADAPT.NOFILTER_CONST_MAX = 3535;
    ADAPT.NORMAL_INITIAL_MIN = 3540;
    ADAPT.NORMAL_CONST_MIN = 3550;
    ADAPT.NORMAL_CONST_MAX = 3595;
    ADAPT.NORMAL_INITIAL_MAX = 3610;
    ADAPT.NOAIR_CONST_MIN = 3615;
    ADAPT.ABSOLUTE_MAX = 3700;
    ADAPT.LOWER_DIF = 25;
    ADAPT.UPPER_DIF = 15;
    ADAPT.MAX_ACCEL = 0.65;
  }
  else
  {
    Serial.println("SSID not found");
    return false;
  }
  Serial.println("Adaptive values loaded for" + loadedSSID);
  //saveAdaptiveSettings(ADAPT);
  return true;
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
    settings.NORMAL_CONST_MIN = inputCommand.substring(19).toInt();
    Serial.print("New NORMAL_CONST_MIN set to: ");
    Serial.println(settings.NORMAL_CONST_MIN);
  }
  else if (inputCommand.startsWith("set normalConstMax: "))
  {
    settings.NORMAL_CONST_MAX = inputCommand.substring(19).toInt();
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
    saveAdaptiveSettings(settings);
    inputCommand = ""; // Clear the command
  }
  return returnValue; // Successfully set a setting
}
