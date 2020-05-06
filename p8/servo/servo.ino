#include <Servo.h>

Servo servo;

int i = 0;

void setup(){
    servo.attach(9);
}

void loop(){
    // Asigna posición a servo
    servo.write(map(i, 0, 1023, 0, 179));

    //i = (i+1) % 1024;
    if(i==0){
      i = 1023;  
    }else{
      i = 0;  
    }

    // Delay para que se ajuste
    delay(1200);
}
