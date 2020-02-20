#include <MsTimer2.h>

#define debounce_ms 50

const int red_led = 10;
const int green_led = 5;
const int button = 3;



const int time_range_ms[] = {400, 1200};

int contador;

void setup(){
  // PinMode Set
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(button, INPUT);

  // Enable serial interface
  Serial.begin(115200);

  // Leer la hora eliminando los ':'
  Serial.parseInt(SKIP_ALL, ':');
}

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

bool countdown(int contador){
  while(--contador){
    if(digitalRead(button) == HIGH){
      return false;
    }
    // Esperamos un ms
    delay(1);
  }
  return true;
}

void loop(){

  // Ready
  ledWrite(HIGH, LOW);

  // Wait for ONE input (no debouncing affects here)
  while (digitalRead(button) == LOW){
    // ¯\_(ツ)_/¯
  }

  // Wait until button release
  while (debounce_digitalRead(button) == HIGH){
    // ¯\_(ツ)_/¯
  }



  // Steady
  ledWrite(LOW, LOW);



  // Wait random period of time. OJO min inclusive, max exclusive (+1)
  contador = random(time_range_ms[0], time_range_ms[1]+1);

  if(countdown(contador)){
    // Fire
    ledWrite(LOW, HIGH);

    // Get current time
    time = millis();
    while (digitalRead(button) == LOW){
      // ¯\_(ツ)_/¯
    }
    // Get time difference
    time = millis() - time;

    // Apagamos led rojo
    ledWrite(LOW, LOW);

    // Print time
    Serial.print(time);
    Serial.print("ms\t");

    // Print tier message
    if(time < 100){
      Serial.println("Enhorabuena! Excelentes reflejos!");
    } else if(time > 200){
      Serial.println("Intente de nuevo...");
    } else {
      Serial.println("Bien! Buenos reflejos!");
    }

  } else {
    Serial.println("No se precipite!");
  }

  // Wait for button release
  while (debounce_digitalRead(button) == HIGH){
    // ¯\_(ツ)_/¯
  }

}
