//? SETTINGS
bool serialEnabled = false;
bool soundEnabled = true;

//? DOWNLOADED LIBRARIEs
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include <analogWrite.h>
#include <pitches.h>
#include <Tone32.h>
#include "Wire.h"
#include "DFRobot_BME280.h"
#include <DFRobot_ENS160.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <nvs_flash.h>

//? CUSTOM LIBRARIEs
#include "hardware\Audio.h"
#include "hardware\Device.h"
#include "hardware\Fan.h"
#include "hardware\Lamp.h"
#include "hardware\Battery.h"
#include "hardware\Piezo.h"
#include "hardware\reed.h"
#include "hardware\touchInput.h"
#include "hardware\infraredSensor.h"
#include "hardware\tacho.h"

#include "Timer.h"
#include "movingAverage.h"
#include "Interpreter.h"
#include "FunctionRunner.h"
#include "stat_data_struct.h"
#include "sensor_data_struct.h"
#include <algorithm>
#include "Webpage.h"

#include "onTimeTracker.h"

#include "Majority.h"
#include "Acceleration.h"
//? DATA STORAGE
Preferences data;
bool accelPrintEnable = false;
unsigned long durationUpdate;
//? WIFI
const char *default_ssid = "HeadShield";
const char *password = "PAPR_user_2023";
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

//? SENSOR
SensorData perkData;
#define SEA_LEVEL_PRESSURE 1015.0f
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
DFRobot_BME280_IIC BME280(&Wire, 0x76);
bool connectCommand = false;
bool sensorConnected = false;

//? FAN
const int fanPin = 5;
Fan fan(fanPin);

unsigned long autoUpdateFilterTime = 10 * 60 * 1000;
unsigned long saveMaxPeriodTime = 4 * 60 * 1000;
OnTimeTracker filterTracker(saveMaxPeriodTime);

//? POWER LED
const int LEDPin = 19;
LED lamp(LEDPin);

//? TACHOMETER
const int tachometerPin = 39;
Tachometer tacho(tachometerPin, 200);
float accelerationValue;
float accelerationMax;
float accelerationMin;

//? BATTERY
const int batteryPin = 32;
Battery battery(batteryPin, 300);
unsigned int batteryPercentage = 0;

//? HEAD SENSOR
const int infraredPin = 35;
InfraredSensor headSensor(infraredPin, 30);

//? VISOR
const int reedSwitchPin = 18;
ReedSwitch visor(reedSwitchPin, 40);

//? TOUCH INPUT
const int touchRightPin = 33;
const int touchLeftPin = 15;
Touch touchRight(touchRightPin, 0, 24);
Touch touchLeft(touchLeftPin, 0, 50);

//? PIEZO
const int piezoPin = 23;
const int piezoChannel = 8;
Piezo piezo(piezoPin, piezoChannel);

//? AUDIO
const int audioEnPin = 16;
Audio audio(audioEnPin);

//? GLOBAL
struct AdaptiveValues
{
  unsigned int ABSOLUTE_MIN;
  unsigned int NOFILTER_CONST_MAX;
  unsigned int NORMAL_INITIAL_MIN;
  unsigned int NORMAL_CONST_MIN;
  unsigned int NORMAL_CONST_MAX;
  unsigned int NORMAL_INITIAL_MAX;
  unsigned int NOAIR_CONST_MIN;
  unsigned int ABSOLUTE_MAX;
  unsigned int LOWER_DIF;
  unsigned int UPPER_DIF;
  float MAX_ACCEL;
};
FanCondition normal;
AdaptiveValues ADAPT;
cond::conditionNumber fanErrorNumber;

void setInitialLimits()
{
  normal.setMax(ADAPT.NORMAL_INITIAL_MAX);
  normal.setMin(ADAPT.NORMAL_INITIAL_MIN);
}
#include "basic.h"
#include "webInterface.h"
#include "interpreterCommands.h"
#include "sensorConnection.h"

void setup()
{
  //* BEGIN
  Serial.begin(115200);
  fan.begin();
  lamp.begin();
  audio.begin();
  piezo.begin(soundEnabled);
  visor.begin();
  battery.begin();
  tacho.begin();
  filterTracker.begin();
  //* RETRIEVE DATA
  restoreCondition(normal, data, "normal");
  String restoredSSID = restoreWifiCredentials(data);
  const char *ssidChar = restoredSSID.c_str();
  restoreBatteryCorrection();
  //* WIFI
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, local_ip, subnet);
  WiFi.softAP(ssidChar, password);
  serverOn();
  server.begin();
  AsyncElegantOTA.begin(&server);

  //* SENSOR
  BME280.reset();
  connectCommand = true;
  Timer connection(3000);
  while (!connection.timeElapsedMillis())
  {
    if (sensorConnectRequest())
      break;
  }

  ENS160.setPWRMode(ENS160_STANDARD_MODE);
  ENS160.setTempAndHum(25.0, 50.0);

  //* start up
  piezo.playStartup();
  loadAdaptiveSettings(data, ADAPT);
  setInitialLimits();
}

bool multiTouch()
{
  bool returnValue = false;
  const unsigned int timeMax = 2000;
  const unsigned int timeMin = 1000;
  static unsigned long timeWhenStart = 0;
  static bool timeStarted = false;

  if (touchLeft.getDigital() && touchRight.getDigital())
  {
    if (!timeStarted)
    {
      timeWhenStart = millis();
      timeStarted = true;
    }

    if (millis() - timeWhenStart > timeMin)
    {
      timeStarted = false;
      returnValue = true;
    }
  }
  else
  {
    timeWhenStart = 0;
    timeStarted = false;
  }

  return returnValue;
}
void touchInputHandler()
{
  if (touchLeft.longTap() && !touchRight.getDigital()) //? FAN TOGGLE
  {
    fan.toggle();
    if (fan.active())
      piezo.playFanOn();
    else
      piezo.playFanOff();
  }

  if (touchRight.singleTap() && !touchLeft.getDigital()) //? LED CONTROL
  {
    lamp.toggleBetween(0, 3);

    if (lamp.level == 0)
      piezo.playLampOff();
    else
      piezo.playLampOn();
  }

  if (touchRight.longTap() && !touchLeft.getDigital()) //? AUDIO CONTROL
  {
    audio.toggle();

    if (audio.state)
      piezo.playVisorUp();
    else
      piezo.playVisorDown();
  }

  if (touchLeft.singleTap() && !touchRight.getDigital()) //? EMPTY
  {
  }
  if (multiTouch()) //? EMPTY
  {
  }
}

void updateSensor()
{
  perkData.temp = BME280.getTemperature();
  perkData.press = BME280.getPressure();
  perkData.humi = BME280.getHumidity();
  perkData.status = ENS160.getENS160Status();
  perkData.AQI = ENS160.getAQI();
  perkData.TVOC = ENS160.getTVOC();
  perkData.ECO2 = ENS160.getECO2();
}
int tachoLastValue;
void updateTacho()
{
  if (fan.active())
  {
    tacho.getAverage();
    tacho.getAverage();
    tacho.getAverage();
    tacho.getAverage();
    accountBattery(tacho.finalValue);
  }
}

void accountBattery(int &modifyThis)
{
  int offset = map(battery.percent, 100, 0, 0, 6);
  modifyThis -= offset;
}
void updateBattery()
{
  battery.getPercent();
}

void updateHeadSensor()
{
  headSensor.scan();
}
void updateVisor()
{
  visor.scan();
}

void parseAndAction_tacho()
{
  if (!fan.active())
  {
    fanErrorNumber = cond::off;
    return;
  }

  int value = tacho.finalValue;

  if ((normal.getMin() < value && value < normal.getMax()) || //? NORMAL
      ADAPT.NORMAL_CONST_MIN < value && value < ADAPT.NORMAL_CONST_MAX)
  {
    fanErrorNumber = cond::normal;
  }

  else if (ADAPT.ABSOLUTE_MIN < value && value < normal.getMin() || //? NO FILTER
           value < ADAPT.NOFILTER_CONST_MAX)
  {
    fanErrorNumber = cond::underNormal;
  }
  else if (normal.getMax() < value && value < ADAPT.ABSOLUTE_MAX || //? NO AIR
           value > ADAPT.NOAIR_CONST_MIN)
  {
    fanErrorNumber = cond::overNormal;
  }
  else
  {
    fanErrorNumber = cond::other; //? MALFUNCTION
  }

  switch (fanErrorNumber)
  {
  case cond::normal:
    serialPrintIf("NORMAL");
    break;
  case cond::underNormal:
    serialPrintIf("CHECK FILTERS OR FANS");
    break;
  case cond::overNormal:
    serialPrintIf("NOT ENOUGH AIRFLOW");
    break;
  case cond::other:
    serialPrintIf("AIRFLOW SYSTEM MALFUNCTION");
    break;
  }

  static Timer makeWarningTimer(10000);

  if (tacho.warning)
  {
    MajorityResult newResult = getMajority(fanErrorNumber);
    if (newResult.isMajority && newResult.majorityValue != cond::normal)
    {
      Serial.println("Last five values were not normal");
      if (makeWarningTimer.timeElapsedMillis())
      {
        Serial.println("Warning signal");
        //airflowSystemWarning();
      }
    }
    else
    {
      makeWarningTimer.preTime = millis();
    }
  }
}
void airflowSystemWarning()
{
  fan.off();
  delay(300);

  int lampLevel = lamp.level;
  lamp.level = 0;

  for (int i = 0; i < 5; i++)
  {
    lamp.toggleBetween(0, 3);
    delay(20);
  }
  piezo.playError();
  lamp.setLevel(lampLevel);
  fan.on();
}

void parseAndAction_battery()
{
  batteryPercentage = battery.percent;
  static unsigned int prevPercent = battery.percent;

  if (batteryPercentage > prevPercent)
    batteryPercentage = prevPercent;

  prevPercent = batteryPercentage;
}

void parseAndAction_headSensor()
{
  static bool onDone = false;
  static bool offDone = false;

  switch (headSensor.state)
  {
  case 1:
    if (!onDone)
    {
      piezo.playHelmetPutOn();
      serialPrintIf("HEAD SENSOR ON");
      onDone = true;
      offDone = false;

      if (visor.state)
        fan.on();
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playHelmetTakenOff();
      serialPrintIf("HEAD SENSOR OFF");
      offDone = true;
      onDone = false;
      fan.off();
      lamp.off(); //! CHECK

      static unsigned long headSensorSaveStart = 0;

      if (millis() - headSensorSaveStart > 120000)
      {
        if (filterTracker.save(true))
        {
          headSensorSaveStart = millis();
        }
      }
    }
    break;
  }
}
void parseAndAction_visor()
{
  static bool onDone = false;
  static bool offDone = false;

  switch (visor.state)
  {
  case 1:
    if (!onDone)
    {
      piezo.playVisorFoldedDown();
      serialPrintIf("VISOR ON");
      onDone = true;
      offDone = false;

      fan.on();
    }
    break;
  case 0:
    if (!offDone)
    {
      piezo.playVisorFoldedUp();
      serialPrintIf("VISOR OFF");
      offDone = true;
      onDone = false;

      fan.off();
    }
    break;
  }
}

void updateFilterTracker()
{
  filterTracker.update(fan.state);

  static unsigned long previousOnTime = 0;
  if (filterTracker.get_timeOn() - previousOnTime > autoUpdateFilterTime)
  {
    filterTracker.save();
    previousOnTime = filterTracker.get_timeOn();
  }
}

void adjustThresholds(int tachoValue, FanCondition &newThresholds)
{
  static const unsigned int sampleSize = 10;
  static const unsigned int addDuration = 100; // milliseconds
  static const unsigned int periodTime = sampleSize * addDuration;

  static MovingAverage adjustAverage(sampleSize);
  static Acceleration acc;
  static MinMax accelMinMax;

  static unsigned int newAverage = 0;

  if (fan.getCurrentSessionOn() < 7000)
    return;

  if (fanErrorNumber != cond::normal)
  {
    static Timer logOutOfRangeTimer(1000);
    //setInitialLimits();
    if (logOutOfRangeTimer.timeElapsedMillis())
      Serial.println("OUT OF RANGE");
    return;
  }

  static Timer addAccelDataTimer(50);
  if (addAccelDataTimer.timeElapsedMillis())
  {
    acc.addValue(tacho.finalValue);
    accelerationValue = acc.calculateAcceleration(periodTime);
    accelMinMax.addValue(accelerationValue);
    accelerationMin = accelMinMax.getMinValue(periodTime);
    accelerationMax = accelMinMax.getMaxValue(periodTime);
  }

  //* ADD TACHO VALUE TO AVERAGE AND CALCULATE
  static Timer addTimer(addDuration);
  if (addTimer.timeElapsedMillis())
  {
    adjustAverage.add(tachoValue);
    newAverage = adjustAverage.average();
  }

  //* GET NEW THRESHOLDS
  static Timer modifyThresholdTimer(periodTime);
  if ((-ADAPT.MAX_ACCEL < accelerationMin && accelerationMax < ADAPT.MAX_ACCEL) && accelerationValue != -10)
  {
    if (modifyThresholdTimer.timeElapsedMillis())
    {
      int newMax = newAverage + ADAPT.UPPER_DIF;
      int newMin = newAverage - ADAPT.LOWER_DIF;

      if (newMax >= ADAPT.NOAIR_CONST_MIN)
      {
        Serial.println("UPPER LIMIT REACHED");
        newMax = ADAPT.NOAIR_CONST_MIN;
        newMin = ADAPT.NOAIR_CONST_MIN - ADAPT.UPPER_DIF - ADAPT.LOWER_DIF;
      }
      if (newMin <= ADAPT.NOFILTER_CONST_MAX)
      {
        Serial.println("LOWER LIMIT REACHED");
        newMin = ADAPT.NOFILTER_CONST_MAX;
        newMax = ADAPT.NOAIR_CONST_MIN + ADAPT.UPPER_DIF + ADAPT.LOWER_DIF;
      }
      newThresholds.setMax(newMax);
      newThresholds.setMin(newMin);
      Serial.println("EN");
      Serial.println("Tacho: " + String(tachoValue));
    }
  }
  else
  {
    static Timer logDisableAdjustTimer(1000);
    if (logDisableAdjustTimer.timeElapsedMillis())
    {
      Serial.println("DIS");
      Serial.println("Tacho: " + String(tachoValue));
    }
    adjustAverage.clear();
  }
  if (accelPrintEnable)
  {
    Serial.print("Acceleraion: " + String(accelerationValue));
    Serial.print(",");
    Serial.print("Max: " + String(accelerationMax));
    Serial.print(",");
    Serial.println("Min: " + String(accelerationMin));
  }
}

FunctionRunner tachoRunner(parseAndAction_tacho, 1000);
FunctionRunner batteryRunner(parseAndAction_battery, 4000);
FunctionRunner visorRunner(parseAndAction_visor, 100);
FunctionRunner headSensorRunner(parseAndAction_headSensor, 200);
FunctionRunner readSensorRunner(updateSensor, 200);
FunctionRunner readFilterTrack(updateFilterTracker, 1000);
// csökkentsd a period time-ot
void loop()
{
  adjustThresholds(tacho.finalValue, normal);

  readFilterTrack.takeAction();

  AsyncElegantOTA.loop();
  touchInputHandler();

  if (fan.active())
  {
    static Timer updateTachoTimer(50);
    if (updateTachoTimer.timeElapsedMillis())
      updateTacho();
  }
  updateBattery();
  updateHeadSensor();
  updateVisor();

  tachoRunner.takeAction();
  batteryRunner.takeAction();
  visorRunner.takeAction();
  headSensorRunner.takeAction();
  readSensorRunner.takeAction();
  sensorConnectRequest();
  sensorDisconnectRequest();
  sensorReconnectingRequest();

  using namespace interpreter;

  if (Serial.available())
  {
    Serial.println("COMMAND: " + command);
    command = interpreter::getCommand();
  }

  setSSID(command);
  setNormal(command);
  setBatteryParameter(command);
  setAdaptiveSettings(command, ADAPT);

  toggleSerial.refresh(command);
  printTachoValue.refresh(command);
  printPeriferial.refresh(command);

  printLimits.refresh(command);

  printSensorValues.refresh(command);
  analyse_normal.refresh(command);
  clearLimits.refresh(command);

  printFilterTime.refresh(command);
  toggleFan.refresh(command);
  printMemoryWear.refresh(command);
  analyseBattery.refresh(command);
  longTest.refresh(command);
  logAcceleration.refresh(command);
  printAdaptive.refresh(command);
}

/*
  if (fan.active())
  { 
    static Acceleration acc;
    static MinMax accelMinMax;
    acc.addValue(tacho.finalValue);
    accelerationValue = acc.calculateAcceleration(1000);
    accelMinMax.addValue(accelerationValue);
    accelerationMin = accelMinMax.getMinValue(1000);
    accelerationMax = accelMinMax.getMaxValue(1000);
    Serial.print(accelerationValue*1000);
    Serial.print(",");
    Serial.print(accelerationMin*1000);
    Serial.print(",");
    Serial.println(accelerationMax*1000);
  }*/
