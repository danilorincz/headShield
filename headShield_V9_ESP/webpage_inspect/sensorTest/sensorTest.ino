#include "DFRobot_BME280.h"
#include <DFRobot_ENS160.h>
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
struct Timer
{
    unsigned long int preTime;
    float duration;

    Timer(float duration)
    {
        this->preTime = 0;
        this->duration = duration;
    }
    void setDuration(int newDuration)
    {
        this->duration = newDuration;
    }
    bool timeElapsedMicros()
    {
        if (micros() - this->preTime > this->duration)
        {
            this->preTime = micros();
            return true;
        }
        else
        {
            return false;
        }
    }
    bool timeElapsedMillis()
    {
        if (millis() - this->preTime > this->duration)
        {
            this->preTime = millis();
            return true;
        }
        else
        {
            return false;
        }
    }
};

//?  SENSOR
SensorData perkData;
#define SEA_LEVEL_PRESSURE 1015.0f
DFRobot_ENS160_I2C ENS160(&Wire, 0x53);
DFRobot_BME280_IIC BME280(&Wire, 0x76);

void setup()
{
    //* BEGIN
    Serial.begin(115200);

    //* BME280
    BME280.reset();
    if (BME280.begin() != DFRobot_BME280_IIC::eStatusOK)
    {
        perkData.initializeBME280 = false;
        Serial.println("BME280 faild");
    }
    else
    {
        perkData.initializeBME280 = true;
        Serial.println("BME280 sucess");
    }

    //* ENS160
    if (ENS160.begin() != NO_ERR)
    {
        perkData.initializeENS160 = false;
        Serial.println("ENS160 failed");
    }
    else
    {
        perkData.initializeENS160 = true;
        Serial.println("ENS160 sucess");
    }

    ENS160.setPWRMode(ENS160_STANDARD_MODE);
    ENS160.setTempAndHum(25.0, 50.0);
}

bool connectSensor()
{
    BME280.reset();
    if (BME280Connected())
    {
        perkData.initializeBME280 = true;
        Serial.println("BME280 sucess");
    }
    else
    {
        perkData.initializeBME280 = false;
        Serial.println("BME280 faild");
    }

    //* ENS160
    if (ENS160Connected())
    {
        perkData.initializeENS160 = true;
        Serial.println("ENS160 sucess");
    }
    else
    {
        perkData.initializeENS160 = false;
        Serial.println("ENS160 failed");
    }

    ENS160.setPWRMode(ENS160_STANDARD_MODE);
    ENS160.setTempAndHum(25.0, 50.0);
    if (perkData.initializeENS160 && perkData.initializeBME280)
        return true;
    else
        return false;
}

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

//* SENSOR DATA
void refreshSensorData()
{
    perkData.temp = BME280.getTemperature();
    perkData.press = BME280.getPressure();
    perkData.humi = BME280.getHumidity();

    perkData.status = ENS160.getENS160Status();
    perkData.AQI = ENS160.getAQI();
    perkData.TVOC = ENS160.getTVOC();
    perkData.ECO2 = ENS160.getECO2();
}

Timer connectionTimer(4000);
void loop()
{
    if (connectionTimer.timeElapsedMillis())
    {
        if (!perkData.initializeBME280 || !perkData.initializeENS160)
        {
            connectSensor();
        }
        else
        {
            Serial.println("Sensor is connected! **********************");
            refreshSensorData();
            perkData.log();
        }
        if (!BME280Connected())
            perkData.initializeBME280 = false;
        if (!ENS160Connected())
            perkData.initializeENS160 = false;
    }
    // do other stuff...
}
/*
        Serial.println("Time");
        if (!BME280Connected() || !ENS160Connected())
        {
            Serial.println("Sensor is not connected");
            Serial.println("Connecting...");
            connectSensor();
        }
        else
        {
            Serial.println("Already connected");
            perkData.log();
        }*/
