#include "DFRobot_BME680_SPI.h"
#include "SPI.h"
#include "DFRobot_BME680_I2C.h"
#include "Wire.h"

const uint8_t sensor_2_cs = 5;
#define I2Caddress 0x76
#define CALIBRATE_PRESSURE 114

float seaLevel_1;
float seaLevel_2;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  DFRobot_BME680_I2C sensor_1(I2Caddress);
  sensor_1.begin();
  sensor_1.startConvert();
  sensor_1.update();
  long int pressure_1 = sensor_1.readPressure();
  Serial.println(pressure_1);
  /*
  DFRobot_BME680_SPI sensor_2(sensor_2_cs);
  sensor_2.begin();
  sensor_2.startConvert();
  sensor_2.update();
  long int pressure_2 = sensor_2.readPressure() - 30;
  long int difference = pressure_1 - pressure_2;
  Serial.println(difference);*/
}

  /*
    sensor.readTemperature();
    sensor.readPressure();
    sensor.readHumidity();
    sensor.readGasResistance();
    sensor.readAltitude();
    sensor.readCalibratedAltitude(seaLevel_2);
  */
