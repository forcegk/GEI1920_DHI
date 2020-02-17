// Cada pulsación incrementa una unidad un contador
// de pulsaciones y es enviado por el puerto serie.
// Utiliza polling y filtrado de
// rebotes (debouncing) usando millis().
const int pulsador = 2;
const int debounce_time_ms = 50;
int counter = 0;
void setup() {
   Serial.begin(115200);
   pinMode(pulsador, INPUT);
   Serial.println("Start");
}
   
void loop() {
  leePulsador();
  delay(debounce_time_ms + random(-10,10));
}
 
void leePulsador() {
  static unsigned long starttime = millis(); 
  static boolean puls_enable= false;
  if (digitalRead(pulsador)) {
    if (puls_enable) {    
      puls_enable= false;
      if (millis() - starttime > debounce_time_ms) {
        Serial.print ("Count: ");
        Serial.println(++counter);
      }
      starttime = millis();}
  }
  else puls_enable= true;
}
