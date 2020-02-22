#include <MsTimer2.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define debounce_ms 50

const int red_led = 10;
const int green_led = 5;
const int button = 3;

typedef struct _reloj{
  // Con 8 bits sin signo nos llegan
  uint_fast8_t h = 0;
  uint_fast8_t min = 0;
  uint_fast8_t sec = 0;
} reloj;

// Se va a acceder en ISR
volatile reloj r1;
volatile bool power_on = true;
volatile bool led_status = true;

long hora_recibida;
int hora_recibida_h;
int hora_recibida_min;
int hora_recibida_sec;

void set_time(volatile reloj *r, int h, int min, int sec){

  if(h>23 || min>59 || sec>59){
    go_to_sleep();
    return;
  }

  r->h    = (uint_fast8_t) h;
  r->min  = (uint_fast8_t) min;
  r->sec  = (uint_fast8_t) sec;
}

void sumar_uno(){
  // Incrementar segundos
  (r1.sec)++;

  if(r1.sec>59){
    r1.sec = 0;
    // Incrementar minutos
    (r1.min)++;

    if(r1.min>59){
      r1.min = 0;
      // Incrementar horas
      (r1.h)++;

      if(r1.h>23){
        // Reset de horas
        r1.h = 0;
      }
    }
  }

  if(power_on){
    if(r1.h<10){Serial.print(0);} Serial.print(r1.h);Serial.print(":");
    if(r1.min<10){Serial.print(0);} Serial.print(r1.min);Serial.print(":");
    if(r1.sec<10){Serial.print(0);} Serial.println(r1.sec);
    //Serial.print("Estado: "); Serial.println(power_on);
    if(led_status){
      ledWrite(HIGH, LOW);
    }else{
      ledWrite(LOW, HIGH);
    }
  }
  // Actualizamos la posición de los leds
  led_status = !led_status;
}

void pin3_interrupt(){
  if(!power_on){
    sleep_disable();
    // Desactivamos la interruptión
    detachInterrupt(digitalPinToInterrupt(button));
    //Serial.print("Interrupt detached! Status:"); Serial.println(power_on);
    Serial.flush();
    power_on = true;
  }
}

void _sleepNow(){
  power_on = false;
  sleep_enable();
  sleep_mode(); // entra en sleep
}

void go_to_sleep(){
  sleep_enable();
  // Activamos la interrupción para despertar
  attachInterrupt(digitalPinToInterrupt(button), pin3_interrupt, HIGH);

  /*SAFE ZONE*/

    ledWrite(LOW, LOW);

    //Serial.println("Interrupt attached");
    Serial.println("Pasando a Modo Sleep...");
    Serial.flush();

  /*END SAFE ZONE*/

  set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
  cli();
  sleep_bod_disable();
  sei();

  power_on = false;

  while(!power_on){
    _sleepNow();
  }

  sleep_disable();
}

/**************************** PIN READ & LED ****************************/
int debounce_digitalRead(int pin){
  int prev = digitalRead(pin);

  //delay(debounce_ms + random(-10,10));
  delay(debounce_ms);

  return (prev == digitalRead(pin)) ? prev : debounce_digitalRead(pin);
}

void ledWrite(int _green_led_value, int _red_led_value){
  digitalWrite(green_led, _green_led_value);
  digitalWrite(red_led, _red_led_value);
}
/************************************************************************/

void setup(){
  // PinMode Set
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // Enable serial interface
  Serial.begin(115200);

  MsTimer2::set(1000, sumar_uno);
  MsTimer2::start();
}

void loop(){

  if(Serial.available()){
    hora_recibida = Serial.parseInt(SKIP_ALL, ':');

    // Limpiamos la basura que quede después
    while(Serial.peek() != -1){
      Serial.read();
    }

    if(hora_recibida >= 0){
      hora_recibida_sec = hora_recibida % 100;
      hora_recibida_min = hora_recibida % (100*100) / 100;
      hora_recibida_h   = hora_recibida / (100*100);

      //Serial.print("Hora recibida: "); Serial.println(hora_recibida);
      //Serial.print("Estado: "); Serial.println(power_on);
      set_time(&r1, hora_recibida_h, hora_recibida_min, hora_recibida_sec);
    }
  }

}
