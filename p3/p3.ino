#include <Wire.h>

#define debounce_ms 50

#define default_p1 1
#define default_p2 2
#define default_p3 5
#define default_p4 10

#define i2c_address 0x38

/**************************** PIN READ & LED ****************************/
int debounce_digitalRead(int pin){
  int prev = digitalRead(pin);

  //delay(debounce_ms + random(-10,10));
  delay(debounce_ms);

  return (prev == digitalRead(pin)) ? prev : debounce_digitalRead(pin);
}
/************************************************************************/

void i2c_write(uint8_t led_r_l){
  Wire.beginTransmission(i2c_address);

  switch(led_r_l){
    case (uint8_t) 1:
      Wire.write(0b1110);
      break;
    
    case (uint8_t) 2:
      Wire.write(0b1101);
      break;

    case (uint8_t) 3:
      Wire.write(0b1011);
      break;

    case (uint8_t) 4:
      Wire.write(0b0111);
      break;
  }

  Wire.endTransmission();
}

void setup(){
  // Enable serial interface
  Serial.begin(115200);
  Wire.begin();
}

void loop(){
  i2c_write(1);
  delay(1000);
  i2c_write(2);
  delay(1000);
  i2c_write(3);
  delay(1000);
  i2c_write(4);
  delay(1000);

}
