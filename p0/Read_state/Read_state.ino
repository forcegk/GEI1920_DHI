const int pinInput = 3;
const int pinLed = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinInput, INPUT_PULLUP);
  pinMode(pinLed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pinLed, digitalRead(pinInput));
}
