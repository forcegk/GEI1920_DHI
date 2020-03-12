// Genera una onda sinusoidal y la envía al puerto serie
// para visualización en Serial Plotter.
// José J. Lamas. UDC. 2015.

  #include <MsTimer2.h>
  
  // Valores constantes
  const float pi= 3.1416;
  const unsigned int N= 100; // Número de muestras por ciclo
  const unsigned int Ts= 2; // Intervalo entre muestras en ms
  // Período de la señal T= N*Ts (ms)
  
  // Variables globales
  volatile unsigned int n= 0; // Número de muestra (0 a N-1)
  volatile float x; // Valores calculados de función seno + DC  
  
  void setup()  
  {  
    Serial.begin(230400);  
    MsTimer2::set(Ts, generar); // Int. Timer2 cada Ts ms
    MsTimer2::start();
  }  
      
  void loop() { // El bucle principal no hace nada  
  }

  // Rutina de atención de interrupción de Timer2 cada Ts ms
  void generar() {
    x= 2.5 + 2.5*sin(n*2*pi/N); // 2.5 V DC + Seno de 2.5 V amp.    
    if (++n== N) { // Incrementa índice n de muestra
    n= 0;
    }  
    // Envía dato muestra señal cada Ts ms
    Serial.println(x); //  Envía muestra señal a serial plotter
  }
  
