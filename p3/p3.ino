#include <Wire.h>

#define debounce_ms 50

#define default_p1 1
#define default_p2 2
#define default_p3 5
#define default_p4 10
#define premios_no 4  

#define i2c_address 0x38

const uint8_t button = 7;
uint8_t premio;
uint8_t premios[premios_no] = {default_p1, default_p2, default_p3, default_p4};

/**************************** PIN READ & LED ****************************/
int debounce_digitalRead(uint8_t pin){
  uint8_t prev = digitalRead(pin);

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

void do_animation(uint8_t premio){
  
}

void setup(){
  // Enable serial interface
  Serial.begin(115200);
  Wire.begin();

  pinMode(button, INPUT);
}

void loop(){

  while(debounce_digitalRead(button) == LOW){
    // NADA
  }
  while(debounce_digitalRead(button) == HIGH){
    // NADA DE NUEVO
  }

  /* SELECCIÓN DE PREMIO */

  premio = 0; //Reseteamos el valor de premio
  while( premios[(premio = random(1, 1+premios_no))] <= 0 ){}
  premios[premio] -= 1;

  do_animation(premio);
  
  
  i2c_write(1);
  delay(1000);
  i2c_write(2);
  delay(1000);
  i2c_write(3);
  delay(1000);
  i2c_write(4);
  delay(1000);

}
