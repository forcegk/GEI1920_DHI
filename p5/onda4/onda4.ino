#include <MsTimer2.h>

// Valores constantes
const float pi= 3.1416;
const unsigned int N= 200; // Número de muestras por ciclo
const unsigned int Ts= 5; // Intervalo entre muestras en ms
// Período de la señal T= N*Ts (ms)

// Variables globales
volatile unsigned int n= 0; // Número de muestra (0 a N-1)
volatile float x; // Valores calculados de función seno + DC

const float N_4 = (float) N / 4.0;

void setup(){  
  Serial.begin(230400);  
  MsTimer2::set(Ts, generar4); // Int. Timer2 cada Ts ms
  MsTimer2::start();
}  
  
void loop(){ // El bucle principal no hace nada  
}

// Rutina de atención de interrupción de Timer2 cada Ts ms
void generar4(){
  // N_4 = N/4 para menos calculos
  if(0<=n && n<N_4){
    x = 2.5 - 1.25*(-4*(float)n/(float)N);
  }else if(N_4<=n && n<(3*N_4)){
    x = 2.5 + 1.25*(2-(4*(float)n/(float)N));
  }else{
    x = 2.5 - 1.25*(4-(4*(float)n/(float)N));
  }
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}