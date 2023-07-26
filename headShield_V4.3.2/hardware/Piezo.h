#pragma once
#include "Arduino.h"

#ifdef ESP32
#include "analogWrite.h"
#endif
class Piezo
{
private:
  int piezoPin;
  bool state;
  bool logState;
  float speed;

#ifdef ESP32
  int channel;
#endif
public:
  Piezo(int piezoPin, int channel = 0)
  {
    this->piezoPin = piezoPin;
#ifdef ESP32
    this->channel = channel;
#endif
    this->speed = 1.0;
  }

  void begin(bool state = true)
  {
    this->state = state;
    pinMode(piezoPin, OUTPUT);
#ifdef ESP32
    ledcSetup(channel, 2000, 8); // 2000 Hz, 8-bit resolution
    ledcAttachPin(piezoPin, channel);
#endif
  }
  void setSpeed(float percentage)
  {
    this->speed = percentage / 100.0;
  }

  void playTone(unsigned int frequency, unsigned long duration)
  {
    if (state)
    {
      unsigned long adjustedDuration = duration / speed;
#ifdef ESP32
      ledcWriteTone(channel, frequency);
      delay(adjustedDuration);
      ledcWriteTone(channel, 0);
#else
      tone(piezoPin, frequency, adjustedDuration);
#endif
    }
  }

  void playTouch()
  {
    playTone(1000, 50);
  }
  void playFanError()
  {
    playTone(400, 50);
  }

  void playVisorUp()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1500 + i * 200, 50);
      delay(50 / speed);
    }
  }

  void playVisorDown()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(2100 - i * 200, 50);
      delay(50 / speed);
    }
  }

  void playLampOn()
  {
    playTone(800, 50);
    delay(50 / speed);
    playTone(1200, 50);
  }

  void playLampOff()
  {
    playTone(1200, 50);
    delay(50 / speed);
    playTone(800, 50);
  }

  void playBatteryLow()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 - i * 200, 100);
      delay(50 / speed);
    }
  }

  void playBatteryFull()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 + i * 200, 100);
      delay(50 / speed);
    }
  }

  void playFanOn()
  {
    playTone(500, 50);
    delay(50 / speed);
    playTone(800, 50);
  }

  void playFanOff()
  {
    playTone(800, 50);
    delay(50 / speed);
    playTone(500, 50);
  }

  void playFanSpeedUp()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 + i * 100, 50);
      delay(50 / speed);
    }
  }

  void playFanSpeedDown()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1300 - i * 100, 50);
      delay(50 / speed);
    }
  }

  void playError()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(600 - i * 100, 100);
      delay(50 / speed);
    }
  }

  void playSuccess()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(800 + i * 100, 100);
      delay(50 / speed);
    }
  }

  void playStartup()
  {
    playTone(800, 100);
    delay(50 / speed);
    playTone(1000, 100);
    delay(50 / speed);
    playTone(1200, 100);
  }

  void playShutdown()
  {
    playTone(1200, 100);
    delay(50 / speed);
    playTone(1000, 100);
    delay(50 / speed);
    playTone(800, 100);
  }

  void playAudioSystemOn()
  {
    playTone(800, 100);
    delay(50);
    playTone(1200, 100);
    delay(50);
    playTone(1600, 100);
  }

  void playAudioSystemOff()
  {
    playTone(1600, 100);
    delay(50);
    playTone(1200, 100);
    delay(50);
    playTone(800, 100);
  }

  void playVisorFoldedUp()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1600 - i * 200, 50);
      delay(50);
    }
  }
  void playHelmetPutOn()
  {
    for (int i = 0; i < 2; i++)
    {
      playTone(800 + i * 200, 100);
      delay(50);
    }
  }
  void playVisorFoldedDown()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1400 + i * 200, 50);
      delay(50);
    }
  }
  void playHelmetTakenOff()
  {
    for (int i = 0; i < 2; i++)
    {
      playTone(1000 - i * 200, 100);
      delay(50);
    }
  }
};
