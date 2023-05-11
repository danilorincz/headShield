#pragma once
class SensorData
{
public:
  // BMS280
  bool initializeBME280 = false;
  float temp;
  float press;
  float humi;
  // ENS160
  bool initializeENS160 = false;
  uint8_t status;
  uint8_t AQI;
  uint16_t TVOC; //? Total Volatile Organic Compounds
  uint16_t ECO2;
  SensorData()
  {
  }
  void log()
  {
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Pressure: ");
    Serial.println(press);
    Serial.print("Humidity: ");
    Serial.println(humi);

    Serial.print("Status: ");
    Serial.println(status);
    Serial.print("AQI: ");
    Serial.println(AQI);
    Serial.print("TOVC: ");
    Serial.println(TVOC);
    Serial.print("ECO2: ");
    Serial.println(ECO2);
  }
};
