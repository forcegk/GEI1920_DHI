#include <Wire.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <avr/wdt.h>

#define debounce_ms 15

#define default_p1 1
#define default_p2 2
#define default_p3 5
#define default_p4 10
#define default_premios_no 4  

#define i2c_address 0x38

const uint8_t button = 7;
uint8_t premio;
uint8_t premios_no;
uint8_t premios[default_premios_no];
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

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
    case (uint8_t) 0:
      Wire.write(0b1111);
      break;

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
    
    case (uint8_t) 10:
      Wire.write(0b0000);
      break;
  }

  Wire.endTransmission();
}

uint8_t get_prize(){
  if(premios_no==0){
    return 0;
  }

  uint8_t n = random(1, premios_no+1);
  //Debug n=1;

  if(n<=premios[0]){
    return 1;
  } else if(n<=premios[0]+premios[1]){
    return 2;
  } else if(n<=premios[0]+premios[1]+premios[2]){
    return 3;
  } else {
    return 4;
  }

}

void do_animation(uint8_t premio){
  uint8_t saltos_fast = 0;//random(28, 32);
  uint8_t delta_t     = random(20, 24);
  uint16_t v0         = 25;
  float accel         = 0.20;

  uint8_t ajuste      = (saltos_fast + delta_t)%default_premios_no;
  if(ajuste==0){
    ajuste = 4;
  }

  // [ Para la entrega no queremos fase lineal ]
  // Fase lineal
  /*for(uint8_t i = 0; i<saltos_fast; i++){
    i2c_write((i%default_premios_no)+1);
    delay(v0);
  }*/

  // Reseteamos el wd porque no es menos de 8S
  // ya reseteamos dentro del bucle
  //wdt_reset();

  // Fase de deceleración y selección
  for(uint8_t t = 1; t<=(delta_t + (((int)(premio-ajuste))%4)); t++){
    i2c_write( ((((saltos_fast-1%default_premios_no)+1)+t-1)%default_premios_no)+1 );
    wdt_reset();
    delay(((int) round((float) (v0*t)+(float) ((1/2)*accel*t*t))));
  }

  // Reseteamos el wd porque no es menos de 8S
  wdt_reset();

  // Parpadear [para la entrega no hace falta]
  /*for(uint8_t i=0; i<15; i++){
    i2c_write(0);
    delay(100);
    i2c_write(premio);
    delay(100);
  }*/

}

void write_prizes(){
  lcd.setCursor(0, 0);
  lcd.print("P1: "); lcd.print(premios[0]);
  lcd.setCursor(8, 0);
  lcd.print("P2: "); lcd.print(premios[1]);
  lcd.setCursor(0, 1);
  lcd.print("P3: "); lcd.print(premios[2]);
  lcd.setCursor(8, 1);
  lcd.print("P4: "); lcd.print(premios[3]); lcd.print(" ");
}

void setup(){
  wdt_disable();

  // Enable serial interface
  Serial.begin(115200);
  Wire.begin();
  lcd.begin(16, 2);
  analogWrite(6,60);

  randomSeed(analogRead(A5));

  pinMode(button, INPUT);

  if(digitalRead(button) == HIGH){
    EEPROM.update(0, (uint8_t) default_p1);
    EEPROM.update(1, (uint8_t) default_p2);
    EEPROM.update(2, (uint8_t) default_p3);
    EEPROM.update(3, (uint8_t) default_p4);
  };

  premios_no = 0;
  // Cargar los premios
  for(uint8_t i=0; i<4; i++){
    premios_no += ( premios[i]=EEPROM.read(i) );
  }

  i2c_write(0);

  wdt_enable(WDTO_8S);
}

void loop(){

  write_prizes();

  while(debounce_digitalRead(button) == HIGH){
    // (ESTE LO PONEMOS POR SI INICIAMOS CON EL PULSADOR ENCENDIDO)
    wdt_reset();
  }
  while(debounce_digitalRead(button) == LOW){
    // NADA DE NUEVO
    wdt_reset();
  }
  while(debounce_digitalRead(button) == HIGH){
    // NADA OTRA VEZ
    wdt_reset();
    if(!premios_no){
      i2c_write(10);
    }
  }

  // Corrección que no afecta al funcionamiento normal
  // y permite la indicaciión de a premios restantes
  i2c_write(0);

  /* SELECCIÓN DE PREMIO */
  premio = get_prize();

  if(premios_no>0){
    premios_no -= 1;
    premios[premio-1] -= 1;

    do_animation(premio);

    // Actualizamos el valor en la EEPROM;
    EEPROM.update(premio-1, (uint8_t) (EEPROM.read(premio-1)-1));
  }

  wdt_reset();
}
