#include "DFRobot_BME280.h"
#include "DFRobot_BME680_I2C.h"
#include "Wire.h"
#include "DFRobot_CCS811.h"
#include "DFRobot_BME680_SPI.h"
#include "SPI.h"

typedef DFRobot_BME280_IIC BME_I2C;
typedef DFRobot_CCS811 CCS_I2C;
typedef DFRobot_BME680_SPI BME_SPI;

#define SEA_LEVEL_PRESSURE 1015.0f
const int CS_pin = 5;

//? INSIDE
BME_I2C pressureSensorIn(&Wire, 0x77);
CCS_I2C carbonSensor(&Wire, 0x5A);

//? OUTSIDE
BME_SPI pressureSensorOut(CS_pin);

void setup()
{
  Serial.begin(115200);

  pressureSensorIn.reset();
  if (pressureSensorIn.begin() != BME_I2C::eStatusOK)
    Serial.println("inside pressure sensor faild");

  if (pressureSensorOut.begin() != 0)
    Serial.println("outside pressure sensor faild");

  if (carbonSensor.begin() != 0)
    Serial.println("carbon sensor faild");
}

void loop()
{
  Serial.println("======== start print ========");
  //? OUTSIDE PRESSURE
  pressureSensorOut.startConvert();
  delay(1000);
  pressureSensorOut.update();

  Serial.println("OUTSIDE PRESSURE_____________");
  Serial.print("temperature(C) :");
  Serial.println(pressureSensorOut.readTemperature() / 100, 2);
  Serial.print("pressure(Pa) :");
  Serial.println(pressureSensorOut.readPressure());
  Serial.print("humidity(%rh) :");
  Serial.println(pressureSensorOut.readHumidity() / 1000, 2);

  Serial.println(" ");
  Serial.println(" ");
  //? INSIDE PRESSURE
  float temp = pressureSensorIn.getTemperature();
  uint32_t press = pressureSensorIn.getPressure();
  float alti = pressureSensorIn.calAltitude(SEA_LEVEL_PRESSURE, press);
  float humi = pressureSensorIn.getHumidity();

  Serial.println("INSIDE PRESSURE_____________");
  Serial.print("temperature (unit Celsius): ");
  Serial.println(temp);
  Serial.print("pressure (unit pa):         ");
  Serial.println(press);

  Serial.print("humidity (unit percent):    ");
  Serial.println(humi);
  Serial.println(" ");
  Serial.println(" ");
  //? CARBON
  Serial.println("CARBON_____________");
  if (carbonSensor.checkDataReady() == true)
  {
    Serial.print("CO2: ");
    Serial.print(carbonSensor.getCO2PPM());
    Serial.print("ppm, TVOC: ");
    Serial.print(carbonSensor.getTVOCPPB());
    Serial.println("ppb");
  }
  else
  {
    Serial.println("Data is not ready!");
  }
  /*!
     * @brief Set baseline
     * @param get from getBaseline.ino
     */
  //carbonSensor.writeBaseLine(0x447B);

  Serial.println("========  end print  ========");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
  Serial.println(" ");
}
