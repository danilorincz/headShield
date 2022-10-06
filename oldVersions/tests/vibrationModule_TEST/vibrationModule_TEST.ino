const int vibPin = 5;

void setup()
{
  Serial.begin(115200);
  pinMode(vibPin, INPUT);
}

void loop()
{
  Serial.println(digitalRead(vibPin));
  delay(5);

}
