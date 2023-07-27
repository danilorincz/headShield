

//* SENSOR DATA
bool BME280Connected()
{
  if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
    return false;
  else
    return true;
}
bool ENS160Connected()
{
  if (ENS160.begin() != NO_ERR)
    return false;
  else
    return true;
}

bool sensorConnectRequest()
{
  static Timer connectingSensorMax(3000);
  if (connectCommand)
  {
    if (!connectingSensorMax.timeElapsedMillis()) // itt még nézi hogy van-e szenzor
    {
      static bool BME_ok = false;
      static bool ENS_ok = false;
      if (BME280Connected() && !BME_ok)
        BME_ok = true;
      if (ENS160Connected() && !ENS_ok)
        ENS_ok = true;

      if (BME_ok && ENS_ok)
      {
        sensorConnected = true;
        connectCommand = false;
        BME_ok = false;
        ENS_ok = false;
        piezo.playSuccess();
        serialPrintIf("Sensor connected OK");
        return true;
      }
    }
    else // letelt az ido
    {
      connectCommand = false;
    }
  }
  else
  {
    connectingSensorMax.preTime = millis();
  }
  return false;
}
bool isSensorDisconnecting()
{
  if (perkData.AQI == 0 && perkData.TVOC == 252 && perkData.ECO2 == 252)
    return true;
  else
    return false;
}
void sensorDisconnectRequest()
{
  if (sensorConnected)
  {
    if (isSensorDisconnecting())
    {
      sensorConnected = false;
      piezo.playShutdown();
      serialPrintIf("disconnecting sensor");
    }
  }
}
bool isSensorReconnecting()
{
  if (perkData.AQI == 0 && perkData.TVOC == 0 && perkData.ECO2 == 0)
    return true;
  else
    return false;
}
void sensorReconnectingRequest()
{
  if (!sensorConnected)
  {
    if (isSensorReconnecting())
    {
      connectCommand = true;
      serialPrintIf("Reconnecting sensor");
    }
  }
}