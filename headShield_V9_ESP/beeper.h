#pragma once
#include "Arduino.h"

#ifdef ESP32
#include "analogWrite.h"
#endif
class Beeper
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
  Beeper(int piezoPin, int channel = 0)
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

  void playTouchTone()
  {
    playTone(1000, 50);
  }

  void playVisorUpTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1500 + i * 200, 50);
      delay(50 / speed);
    }
  }

  void playVisorDownTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(2100 - i * 200, 50);
      delay(50 / speed);
    }
  }

  void playLampOnTone()
  {
    playTone(800, 50);
    delay(50 / speed);
    playTone(1200, 50);
  }

  void playLampOffTone()
  {
    playTone(1200, 50);
    delay(50 / speed);
    playTone(800, 50);
  }

  void playBatteryLowTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 - i * 200, 100);
      delay(50 / speed);
    }
  }

  void playBatteryFullTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 + i * 200, 100);
      delay(50 / speed);
    }
  }

  void playFanOnTone()
  {
    playTone(500, 50);
    delay(50 / speed);
    playTone(800, 50);
  }

  void playFanOffTone()
  {
    playTone(800, 50);
    delay(50 / speed);
    playTone(500, 50);
  }

  void playFanSpeedUpTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1000 + i * 100, 50);
      delay(50 / speed);
    }
  }

  void playFanSpeedDownTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(1300 - i * 100, 50);
      delay(50 / speed);
    }
  }

  void playErrorTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(600 - i * 100, 100);
      delay(50 / speed);
    }
  }

  void playSuccessTone()
  {
    for (int i = 0; i < 3; i++)
    {
      playTone(800 + i * 100, 100);
      delay(50 / speed);
    }
  }

  void playStartupTone()
  {
    playTone(800, 100);
    delay(50 / speed);
    playTone(1000, 100);
    delay(50 / speed);
    playTone(1200, 100);
  }

  void playShutdownTone()
  {
    playTone(1200, 100);
    delay(50 / speed);
    playTone(1000, 100);
    delay(50 / speed);
    playTone(800, 100);
  }
};
