#include <LiquidCrystal.h>
#include <MsTimer2.h>

#define debounce_ms 15

/**************************** PIN READ & LED ****************************/
int debounce_digitalRead(uint8_t pin)
{
  uint8_t prev = digitalRead(pin);

  //delay(debounce_ms + random(-10,10));
  delay(debounce_ms);

  return (prev == digitalRead(pin)) ? prev : debounce_digitalRead(pin);
}
/************************************************************************/

// Pines botones
const uint8_t boton_polling = 4;
const uint8_t boton_forma   = 2;
const uint8_t potenciometro = A2;

// Valores constantes
const float Vref = 5.0;          // Voltaje de referencia del ADC (por defecto)
const unsigned int rango = 1024; // Rango ADC de Arduino Uno (10 bits)
const float pi = 3.1416;

const unsigned int T = 1;   // Intervalo entre puntos en ms
const unsigned int Tg = 1;  // Intervalo de generación en ms
const unsigned int Ts = 1;  // Intervalo de muestreo en ms

const unsigned int N_step = 100; // Número de suma en puntos por ciclo
const unsigned int N_max = 1000;
volatile unsigned int N = 200; // valor actual (por defecto) de N

const unsigned int sig_total = 4;
volatile unsigned int sig_curr = 1; // valor de la señal actual (default 1)

volatile unsigned int amplitude = 127; // valor de la señal

// Variables globales
volatile unsigned int n = 0;  // Número de punto (0 a N-1)
volatile unsigned int dg = 0; // Divisor de gen. de ms(0 a Tg-1)
volatile unsigned int ds = 0; // Divisor de adq. de ms(0 a Ts-1)
volatile unsigned int A_in;   // Valor adquirido en A0
volatile byte ch1;

volatile byte get_ch1(int curr){
  switch(curr){
    case 1 : return (128 + (int) (((float) amplitude) * sin(n * 2 * pi / N)));
    case 2 :  if(0<=n && n<(N>>2)){
                return 128 - (int) (((float) amplitude)*(    -4.0 * (float)n/(float)N));
              }else if((N>>2)<=n && n<(3*(N>>2))){
                return 128 + (int) (((float) amplitude)*(2.0-(4.0 * (float)n/(float)N)));
              }else{
                return 128 - (int) (((float) amplitude)*(4.0-(4.0 * (float)n/(float)N)));
              }
    case 3 :  if(n<(N>>1)){
                return 128 + amplitude;
              }else{
                return 128 - amplitude;
              }
    case 4 :  return 0+map(amplitude, 0, 127, 0, 255);
  }
}




void setup(){
  Serial.begin(2000000);
  Serial.println("Mat+5:,PWM:");
  Serial.flush();

  // Establecemos el modo de los pines
  pinMode(boton_polling, INPUT_PULLUP);
  pinMode(boton_forma,   INPUT_PULLUP);
  pinMode(potenciometro, INPUT);

  TCCR1B &= 0xF8;               // Pone cero en bits preescaler PWM 9 y 10
  TCCR1B |= 0x01;               // Pone PWM 9 y 10 a frec. de 32 kHz
  MsTimer2::set(T, actualizar); // Int. T2 cada T ms
  MsTimer2::start();
}

void loop(){

  // metemos en while en un if para que no bloquee el programa esperando a la pulsación
  if(digitalRead(boton_polling) == LOW){
      while(digitalRead(boton_polling) == LOW){
        // Esperamos a que suelte el botón
        // no hacemos debouncing, porque con la cantidad
        // de interrupciones que hay, delay acaba durando varios segundos
      }

      //Cambiamos valor de polling
      N = (N % N_max) + N_step;
  }

  if(digitalRead(boton_forma) == LOW){
      while(digitalRead(boton_forma) == LOW){
        // NA DE NA
      }

      //Cambiamos forma de onda
      sig_curr = (sig_curr%sig_total)+1;
  }

}

// Rutina de atención de interrupción de T2 cada T ms
void actualizar(){
  amplitude = map(analogRead(potenciometro), 0, 1023, 0, 127);

  if (++dg == Tg){
    dg = 0;                                // Salidas PWM para generación cada 'Tg*T' ms
    ch1 = get_ch1(sig_curr);               // Calcula punto de CH1
    analogWrite(9, ch1);                   // Salida de CH1 por pin3 PWM
  }
  if (++n >= N){ // Incrementa número de punto (cambiado a >= por si hay cambio on the fly)
    n = 0;
  }
  if (++ds == Ts){
    ds = 0;               // Aquisición de canal cada 'Ts*T' ms
    A_in = analogRead(0); // Adquiere A0
  }
  // Envía dato cada T ms a Serial Plotter
  Serial.print(5 + 4 * ch1 * Vref / rango); // Envía voltaje matemático/4
  Serial.print(" ");
  Serial.println(A_in * Vref / rango); // Envía voltaje canal A0
}