#include <MsTimer2.h>

// Valores constantes
const float pi= 3.1416;
const unsigned int N= 100; // Número de muestras por ciclo
const unsigned int Ts= 5; // Intervalo entre muestras en ms
// Período de la señal T= N*Ts (ms)

// Variables globales
volatile unsigned int n= 0; // Número de muestra (0 a N-1)
volatile float x; // Valores calculados de función seno + DC

const float N_5 = (float) N / 5.0;

void setup(){  
  Serial.begin(230400);  
  MsTimer2::set(Ts, generar5); // Int. Timer2 cada Ts ms
  MsTimer2::start();
}  
  
void loop(){ // El bucle principal no hace nada  
}

// Rutina de atención de interrupción de Timer2 cada Ts ms
void generar5(){
  // N_4 = N/4 para menos calculos
  if(n<N_5){
    x = 5;
  }else{
    x = 0;
  }
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}