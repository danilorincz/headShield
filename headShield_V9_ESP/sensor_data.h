#pragma once
class SensorData
{
public:
  float temp;
  float pressure;
  float humi;
  int ppm;  //? Parts Per Million
  int tovc; //? Total Volatile Organic Compounds
  SensorData()
  {
    
  }
  void log()
  {
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.print("Humidity: ");
    Serial.println(humi);
    Serial.print("PPM: ");
    Serial.println(ppm);
    Serial.print("TOVC: ");
    Serial.println(tovc);
  }
};