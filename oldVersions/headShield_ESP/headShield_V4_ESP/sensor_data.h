#pragma once
struct SensorData
{
public:
  float temp;
  float pressure;
  float humi;
  int ppm;  //? Parts Per Million
  int tovc; //? Total Volatile Organic Compounds
};