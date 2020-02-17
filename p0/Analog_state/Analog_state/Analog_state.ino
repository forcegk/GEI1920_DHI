const int anaPin = 0;
const int PWMPin = 10;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(PWMPin, analogRead(anaPin)/4);
}
