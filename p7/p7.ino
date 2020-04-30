#include <MsTimer2.h>

//#define DEBUG

const double A_param = 1.11492089e-3;
const double B_param = 2.372075385e-4;
const double C_param = 6.954079529e-8;
const double k_param = 2.5; // mW/C

const unsigned int temp_avg = 5; // promedidado de las temp_avg
                                 // ultimas temperaturas

const unsigned int Ns = 100;    // Numero de muestras
const unsigned int Ts = 2;      // Intervalo de muestreo en ms
const unsigned int Tp = Ns*Ts;  // Intervalo entre puntos en ms

// Asignación de pines a segmentos
const byte segs[7]= {3,4,5,6,7,8,9}; //a-b-c-d-e-f-g
// Asignación de pines a dígitos
const byte digs[4]= {10,11,12,13}; // 1-2-3-4
// Segmentos que encienden cada dígito del 0 al 9
const byte digitos[10]= { // 0 -> 4, 5 -> 9
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};
const byte grado   = 0b01100011;
const byte letra_C = 0b00111001;
const byte menos   = 0b01000000;

// Variables globales
volatile byte bytes[]= {menos, 0, digitos[0], grado, letra_C};  // layout deslizante

volatile float temperatures[temp_avg];
volatile unsigned int temp_curr = 0; // numero de temperatura actual
volatile float temperature; // temperatura promediada de las ultimas temp_avg

volatile unsigned int n = 0;    // numero de muestra actual

/********************************* INTERRUPT *********************************/
// Interrupción de T2 llamada cada 4 ms para multiplexado del display
void multiplexar() {
  static int digito = 0; // Dígito encendido (D1 a D4);

  for (int i= 0; i<= 3; i++) {
    digitalWrite(digs[i], HIGH); // Apaga los 4 dígitos
  }

  segmentos(bytes[digito+((temperature>-10.0))]); // Saca segmentos digito con offset
  digitalWrite(digs[digito], LOW); // Enciende dígito activo
  digito = (digito+1) % 4;
}
void segmentos(byte valor) {
  for(int seg= 0; seg< 7; seg++) {
    digitalWrite(segs[seg], bitRead(valor, seg));
  }
}


void isr(){
  static double Rm;
  static double logRm;
  static double tempK;
  static double Vm;
  static unsigned int measurement;
  static int temperature_int;

  if(n==0){
    temperatures[temp_curr] = 0.0;
  }

  measurement = analogRead(A0); // guardamos el valor actual de A0

  // Calculamos la parte n'esima de la media de la temperatura en kelvin
  Rm = (double) ((unsigned long long) 10000 * (unsigned long long) measurement) / ( 1024-measurement ); 

  /*testbench*/
  #ifdef DEBUG
    if(millis()%25000<5000){
      Rm = 80000;
    }else if(millis()%25000<10000){
      Rm = 40000;
    }else if(millis()%25000<15000){
      Rm = 33000;
    }else if(millis()%25000<20000){
      Rm = 23000;
    }else if(millis()%25000<25000){
      Rm = 9000;
    }
  #endif // DEBUG

  logRm = log(Rm);
  Vm = (double) 5.0 * (double) measurement / 1024.0;
  tempK = (double) 1.0 / (A_param + B_param*logRm + C_param*logRm*logRm*logRm);
  temperatures[temp_curr] += ( tempK - ((1000.0 * Vm * Vm) / (k_param*Rm) ) ) / (double) Ns;

  // multiplexamos cada 4 ms, valor proximo al ejemplo de 5ms, no tiene
  // más razón. Podríamos hacerlo cada interrupción sin problema.
  if(n%2){
    multiplexar();
  }

  if(++n == Ns){
    // Promediamos las ultimas 5 temperaturas
    temperature = 0.0;
    for(unsigned int i = 0; i<temp_avg; i++){
      temperature += (double) temperatures[i] / (double) temp_avg;
    }
    temperature -= 273.15; // corregimos a celsius

    // Cambiamos el indice del array circular
    temp_curr = (temp_curr+1) % temp_avg;

    // Cambiar valor de bytes en el display
    temperature_int = (int) round(temperature);
    
    bytes[1] = digitos[abs((temperature_int/10)%10)]; // modulo 10 por si sube de 100
    bytes[2] = digitos[abs(temperature_int%10)];

    if(abs(temperature_int) < 10){
      if(temperature_int < 0){
        bytes[1] = menos;
      }else{
        bytes[1] = 0;
      }
    }

    // Imprimir información
    Serial.print("Time(ms): "); Serial.print(millis());
    Serial.print("\t Rm(ohm): "); Serial.print(Rm);
    Serial.print("\t T(°C): "); Serial.print(temperature);
    Serial.println();

    // restauramos n al valor inicial (circular)
    n = 0;
  }

}
/******************************* END INTERRUPT *******************************/


// Configuraciones
void setup() {
  for (int pin= 3; pin<= 13; pin++) {
    // Pone modo salidas
    pinMode(pin, OUTPUT);
  }

  // inicializamos temperaturas a cero para no afectar a las medias al principio
  for(unsigned int ta = 0; ta < temp_avg; ta++){
    temperatures[ta] = 273.15; // damos el valor en K
  }

  Serial.begin(230400);

  MsTimer2::set(Ts, isr); // cada Ts ms
  MsTimer2::start();
}

// Bucle principal
void loop() {
  // ¯\_(ツ)_/¯
}