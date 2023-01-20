#pragma once
void bootSound()
{
  tone(piezoPin, NOTE_A4, 100, piezoChannel);
  tone(piezoPin, NOTE_B4, 100, piezoChannel);
  tone(piezoPin, NOTE_C4, 100, piezoChannel);
  tone(piezoPin, NOTE_D4, 100, piezoChannel);
  tone(piezoPin, NOTE_E4, 100, piezoChannel);
  tone(piezoPin, NOTE_F4, 100, piezoChannel);
}

void beepForward()
{
  tone(piezoPin, NOTE_C4, 100, piezoChannel);
}

void quitBeep()
{
  tone(piezoPin, NOTE_F4, 100, piezoChannel);
  tone(piezoPin, NOTE_E4, 100, piezoChannel);
}

void beepTurnOn()
{

  tone(piezoPin, NOTE_A4, 100, piezoChannel);
  tone(piezoPin, NOTE_B4, 100, piezoChannel);
}
void beepTurnOff()
{
  tone(piezoPin, NOTE_B4, 100, piezoChannel);
  tone(piezoPin, NOTE_A4, 100, piezoChannel);
}
