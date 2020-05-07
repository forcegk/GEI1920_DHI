#include <Servo.h>

//#define __DEBUG__

#ifndef DELAY_180_MS
#define DELAY_180_MS 300
#endif

// Pines
const uint8_t ldr_izq = A0;
const uint8_t ldr_der = A1;

// Parametros
const uint8_t err_min = 10;
const uint8_t n_lecturas = 40;

// Variables de control
Servo servo;
double kp, ki, kd;
double err_cum = 0.0;

// Variables de loop
uint8_t angulo = 0;
#ifdef __DEBUG__
uint8_t prev_angulo = 0;
#endif

/* Al final no hizo falta, pero la dejo aquí porque es útil para veces siguientes
// Función de mapeado de doubles
double map_double(double x, double in_min, double in_max, double out_min, double out_max){
    return ( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min );
}
*/

// Configuraciones
void setup(){
    // pinModes
    pinMode(ldr_izq, INPUT);
    pinMode(ldr_der, INPUT);

    kp = ki = kd = 0.03;

    #ifdef __DEBUG__
    Serial.begin(250000);
    #endif

    servo.attach(9);
    servo.write(angulo = 0); // lo ponemos a 0 grados
    delay(DELAY_180_MS); // le damos el tiempo necesario para girar hasta 180 grados
}

double medir(){
    double err_diff = 0;
    double err_izq, err_der;
    
    for(uint8_t n = 0; n < n_lecturas; n++){
        // damos tiempo para la medida
        err_izq = analogRead(A0);
        delayMicroseconds(500);
        err_der = analogRead(A1);
        delayMicroseconds(500);

        // Calculamos la media (resultado parcial de cada vez)
        err_diff += (err_izq - err_der) / (double) n_lecturas;
    }

    return err_diff;
    
}

uint8_t pir(){
    static double last_err; // static se inicializa automaticamente a cero
    double err, err_tmp, new_angulo;

    err = medir();
    
    if(abs((int) floor(err)) < err_min){
        return angulo;
    }else{
        err_cum += err;
        err_cum = constrain(err_cum, (0.0), (179.0/ki));
        
        // Usamos err_tmp para no realizar calculos dentro de la macro contrain()
        // (es la recomendación que pone en la documentación de arduino así que por si acaso)
        
        err_tmp = ki*err_cum; // No nos hace falta hacer constrain porque lo hacemos arriba
        err_tmp += kp*err;
        err_tmp += kd*(err-last_err);

        new_angulo = constrain(err_tmp, 0.0, 179.9);
        
        last_err = err;
        
        return (uint8_t) floor(new_angulo);
    }
}


void loop(){
    #ifdef __DEBUG__
    prev_angulo = angulo;
    #endif

    servo.write(angulo = pir());

    #ifdef __DEBUG__
    if(angulo != prev_angulo){
        Serial.print("Angulo: ");
        Serial.println(angulo);
    }
    #endif

    delay(40);
}