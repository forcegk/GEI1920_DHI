#include <LiquidCrystal.h>
#include "lcd_icons.h"

#define debounce_ms 15

#define HAPPY_MONKEY 1
#define TRAPPED_MONKEY 2
#define DEAD_MONKEY 3

#define MONKEY 1
#define TRAP 2
#define TIGER 3
#define BANANAS 4
#define MONKEY_TRAPPED 5

const uint8_t SW_pin = 7; // digital pin connected to switch output
const uint8_t X_pin = 0;  // analog pin connected to X output
const uint8_t Y_pin = 1;  // analog pin connected to Y output

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

uint8_t time;
bool grid[16][2];
uint8_t trap_locations[] = {2, 4, 5, 6, 8, 9, 11, 12, 13, 14};
uint8_t time_div;

bool playing;

typedef struct _mono{
  uint8_t x;
  uint8_t y;
} mono;
typedef mono platanos;
typedef mono tigre;

mono m;
platanos plat;
tigre tig;

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

void happy_screen(){
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("H A P P Y");
  lcd.setCursor(2, 1);
  lcd.print("M O N K E Y");
}

void trapped_screen(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("T R A P P E D");
  lcd.setCursor(2, 1);
  lcd.print("M O N K E Y");
}

void dead_screen(){
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("D E A D");
  lcd.setCursor(2, 1);
  lcd.print("M O N K E Y");
}

void read_input(){
  uint16_t x_value = analogRead(X_pin);
  uint16_t y_value = analogRead(Y_pin);

  // Desplazamos al mono en X
  if(!grid[m.x][m.y]){
    // se ha cambiado 412 por 388 porque es el simetrico y mejora la experiencia de juego
    if(x_value < 388){
      if(m.x > 1) m.x--;
    } else if(x_value > 612){
      if(m.x < 15) m.x++;
    }

    // Desplazamos al mono en Y
    if(y_value < 388){
      m.y = 0;
    } else if(y_value > 612){
      m.y = 1;
    }
  }
}

bool monkeyIsTrapped(){
  return grid[m.x][m.y];
}

void redraw(){
  lcd.clear();

  // Tenemos que colocar esto aquí para que detecte los movimientos por 
  // lectura y no cada vez que se actualizan las trampas

  // REALMENTE HACE FALTA, O ME HE EQUIVOCADO YO?
  /* SI HAY QUE ACTUALIZAR PLATANOS Y TIGRE CADA VEZ QUE SE case
      EN TRAMPA, DESCOMENTAR ESTE SEGMENTO */
  /*
  if(monkeyIsTrapped()){
    tig.y = m.y;
    plat.y = (m.y+1)%2;
  }
  */

  for(uint8_t j=0; j<2; j++){
    //mostramos el tiempo
    lcd.setCursor(0, j);
    lcd.print(j==0? time/10 : time%10);

    //dibujamos por lineas
    for(uint8_t i=0; i<(sizeof(trap_locations)/sizeof(uint8_t)); i++){
      lcd.setCursor(trap_locations[i], j);

      if(grid[trap_locations[i]][j])
        lcd.write(TRAP);
      else
        lcd.print(" ");
    }

    //pintamos tigre y bananas
    lcd.setCursor(15, j);
    lcd.write(j==tig.y? TIGER : BANANAS);
  }

  lcd.setCursor(m.x, m.y);
  if(monkeyIsTrapped()){
    lcd.write(MONKEY_TRAPPED);
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

uint8_t get_monkey_status(){
  if((m.x==tig.x) && (m.y==tig.y)){
    return DEAD_MONKEY;
  }else
  if((m.x==plat.x) && (m.y==plat.y)){
    return HAPPY_MONKEY;
  }
  else{
    return TRAPPED_MONKEY;
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

  //Inicializamos el grid a falses (por si acaso)
  for(uint8_t i=0; i<16; i++){
    for(uint8_t j=0; j<2; j++){
      grid[i][j] = false;
    }
  }

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
  m.x = 1;
  tig.x = plat.x = 15;
  m.y = tig.y = 0;
  plat.y = 1;

  playing = true;
  time = 30;
  time_div = 0;

  update_traps();

  while(playing){
    read_input();

    if((time_div=(time_div+1)%8)==0){
      time--;
      if(time!=0){
        update_traps();
      }
    }

    if(!time){
      playing = false;
    } else {
      redraw();
      delay(1000/8);
    }

    if(m.x == 15){
      playing = false;
    }

  }

  switch(get_monkey_status()){
    case HAPPY_MONKEY:
      happy_screen();
      break;
    case TRAPPED_MONKEY:
      trapped_screen();
      break;
    case DEAD_MONKEY:
      dead_screen();
      break;
  }

  while(debounce_digitalRead(SW_pin) == LOW){
    // NADA (ESTE LO PONEMOS POR SI INICIAMOS CON EL PULSADOR PULSADO)
  }
  while(debounce_digitalRead(SW_pin) == HIGH){
    // NADA DE NUEVO
  }
  while(debounce_digitalRead(SW_pin) == LOW){
    // NADA OTRA VEZ
  }
  
}