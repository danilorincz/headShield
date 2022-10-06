
const int irPin = 33;

void setup()
{
  Serial.begin(115200);
  pinMode(irPin, INPUT);

}

void loop()
{
  Serial.print("Analog value: ");
  Serial.println(analogRead(irPin));
  delay(400);
}
