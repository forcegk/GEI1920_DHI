#include <LiquidCrystal.h>
#include "lcd_icons.h"

#define debounce_ms 50

#define HAPPY_MONKEY 1
#define TRAPPED_MONKEY 2
#define DEAD_MONKEY 3

#define MONKEY 1
#define TRAP 2
#define TIGER 3
#define BANANAS 4
#define MONKEY_TRAPPED 5

const uint8_t SW_pin = 8; // digital pin connected to switch output
const uint8_t X_pin = 0;  // analog pin connected to X output
const uint8_t Y_pin = 1;  // analog pin connected to Y output

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

uint8_t time;
bool grid[15][2];
uint8_t trap_locations[] = {2, 4, 5, 6, 8, 9, 11, 12, 13, 14};

bool playing;
uint8_t status;

typedef struct _mono{
  uint8_t x;
  uint8_t y;
} mono;

mono m;

/**************************** PIN READ & LED ****************************/
int debounce_digitalRead(uint8_t pin){
  uint8_t prev = digitalRead(pin);

  //delay(debounce_ms + random(-10,10));
  delay(debounce_ms);

  return (prev == digitalRead(pin)) ? prev : debounce_digitalRead(pin);
}
/************************************************************************/

void init_screen(){
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("MONKEY TRAPS");
  lcd.setCursor(1, 1);
  lcd.print("Press Joystick");
}

void read_joystick(){
  uint16_t x_value = analogRead(X_pin);
  uint16_t y_value = analogRead(Y_pin);

  // Desplazamos al mono en X
  if(x_value < 412){
    if(m.x > 0) m.x--;
  } else if(x_value > 612){
    if(m.x < 15) m.x++;
  }

  // Desplazamos al mono en Y
  if(y_value < 412){
    m.y = 0;
  } else if(x_value > 612){
    m.y = 1;
  }
}

void redraw(){
  for(uint8_t i=0; i<(sizeof(trap_locations)/sizeof(uint8_t)); i++){
    for(uint8_t j=0; j<2; j++){
      lcd.setCursor(trap_locations[i], j);

      if(grid[trap_locations[i]-1][j])
        lcd.write(TRAP);
      else
        lcd.print(" ");
    }
  }

  lcd.setCursor(m.x+1, m.y);
  if(grid[trap_locations[m.x]][m.y]){
    lcd.write(MONKEY_TRAPPED);
    status = TRAPPED_MONKEY;
  } else {
    lcd.write(MONKEY);
  }
}

void update_traps(){
  uint8_t rand_uint;

  // Update trap location and clear mirror position
  for(uint8_t i=0; i<(sizeof(trap_locations)/sizeof(uint8_t)); i++){
    rand_uint = random(0, 2);
    for(uint8_t j=0; j<2; j++){
      grid[trap_locations[i]][j] = j==rand_uint;
    }
  }
}

void setup(){
  // Enable serial interface
  Serial.begin(115200);
  lcd.begin(16, 2);
  analogWrite(6,60);

  randomSeed(analogRead(A5));

  pinMode(SW_pin, INPUT_PULLUP);

  lcd.createChar(MONKEY, monkey);
  lcd.createChar(TRAP, trap);
  lcd.createChar(TIGER, tiger);
  lcd.createChar(BANANAS, bananas);
  lcd.createChar(MONKEY_TRAPPED, mon_trapped);

  playing = false;
}

void loop(){
  
  init_screen();

  while(debounce_digitalRead(SW_pin) == LOW){
    // NADA (ESTE LO PONEMOS POR SI INICIAMOS CON EL PULSADOR PULSADO)
  }
  while(debounce_digitalRead(SW_pin) == HIGH){
    // NADA DE NUEVO
  }
  while(debounce_digitalRead(SW_pin) == LOW){
    // NADA OTRA VEZ
  }

  // Set de las variables antes de empezar el juego
  lcd.clear();
  m.x = 0;
  m.y = 0;
  playing = true;
  time = 30;
  status = 0;

  while(playing){
    // Liberar al mono si procede 
    status = 0;
    for(uint8_t i=0; i<8; i++){
      redraw();
      if(status!=TRAPPED_MONKEY){
        read_joystick();
      }
      delay(1000/8);
    }

    update_traps();
    time--;


    if(!time){
      playing = false;
    }

  }
  
}
