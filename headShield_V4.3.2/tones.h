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

void forward()
{
  tone(piezoPin, NOTE_C4, 100, piezoChannel);
}

void quit()
{
  tone(piezoPin, NOTE_F4, 100, piezoChannel);
  tone(piezoPin, NOTE_E4, 100, piezoChannel);
}

void turnOn()
{

  tone(piezoPin, NOTE_A4, 100, piezoChannel);
  tone(piezoPin, NOTE_B4, 100, piezoChannel);
}
void turnOff()
{
  tone(piezoPin, NOTE_B4, 100, piezoChannel);
  tone(piezoPin, NOTE_A4, 100, piezoChannel);
}
