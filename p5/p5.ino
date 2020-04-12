#include <MsTimer2.h>

// Valores constantes
const float pi= 3.1416;
const unsigned int N= 200; // Número de muestras por ciclo
const unsigned int Ts= 5; // Intervalo entre muestras en ms
// Período de la señal T= N*Ts (ms)

const unsigned int t_sig     = 5*(1000/Ts); // t*fN == Tiempo de muestra (s) por cada señal
const unsigned int total_sig = 6; // Total de señales

// Variables globales
volatile unsigned int curr_t_sig = 0;
volatile unsigned int curr_sig = 1; // Señal actual. Empezamos en 1
volatile unsigned int n= 0; // Número de muestra (0 a N-1)
volatile float x; // Valores calculados de función seno + DC

const float N_4 = (float) N / 4.0;
const float N_5 = (float) N / 5.0;


void generar1(){
  x= 2.5 + 1.25*sin(2*pi*n/N);
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}

void generar2(){
  x= 2.5 + 1.25*sin(20*pi*n/N);
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}

void generar3(){
  x= 2.5 + (1+0.5*sin(2*pi*n/N))*cos(20*pi*n/N);
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}

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

void generar6(){
  // N_4 = N/4 para menos calculos
  if(n<N_5){
    x = 2.5 + 1.25*cos(20*pi*n/N);
  }else{
    x = 2.5 - 1.25*cos(20*pi*n/N);
  }
  if (++n== N) { // Incrementa índice n de muestra
    n= 0;
  }  
  // Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}

void (*signals[total_sig]) () = {generar1, generar2, generar3, generar4, generar5, generar6};

void setup(){  
  Serial.begin(230400);  
  MsTimer2::set(Ts, generar); // Int. Timer2 cada Ts ms
  MsTimer2::start();
}  
  
void loop(){ // El bucle principal no hace nada  
}

void execute_function( void (*f)(void) ){
  f();
};

// Rutina de atención de interrupción de Timer2 cada Ts ms
void generar(){
  execute_function(signals[curr_sig-1]);

  if (++n== N) { // Incrementa índice n de muestra
    n = 0;
  }

  if(++curr_t_sig == t_sig){
    curr_t_sig = 0;

    if(curr_sig++ == total_sig){
      curr_sig = 1;
    }
  }
// Envía dato muestra señal cada Ts ms
  Serial.println(x); //  Envía muestra señal a serial plotter
}
