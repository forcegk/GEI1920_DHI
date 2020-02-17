const int red_led = 10;
const int green_led = 5;
const int button = 3;

const int debounce_ms = 50;

const int time_range_ms[] = {400, 1200};

int contador;
unsigned long time;

void setup(){
  // PinMode Set
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(button, INPUT);
 
  // Enable serial interface
  Serial.begin(115200);
}

void ready(){
  digitalWrite(green_led, HIGH);
  digitalWrite(red_led, LOW);
}

void steady(){
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, LOW);
}

void fire(){
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, HIGH);
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
  ready();

  // Wait for ONE input (no debouncing affects here)
  while (digitalRead(button) == LOW){
    delay(1);
  }

  // Wait until button release
  while (digitalRead(button) == HIGH){
    // Debounce
    delay(debounce_ms + random(-10,10));
  }



  // Steady
  steady();



  // Wait random period of time. OJO min inclusive, max exclusive (+1)
  contador = random(time_range_ms[0], time_range_ms[1]+1);

  if(countdown(contador)){
    // Fire
    fire();

    // Get current time
    time = millis();
    while (digitalRead(button) == LOW){
      // ¯\_(ツ)_/¯
    }
    // Get time difference
    time = millis() - time;

    // Apagamos led rojo
    digitalWrite(red_led, LOW);

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
  while (digitalRead(button) == HIGH){
    delay(1);
  }

}
