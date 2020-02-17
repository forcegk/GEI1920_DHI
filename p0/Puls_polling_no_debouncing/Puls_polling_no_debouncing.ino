// Cada pulsación incrementa una unidad un contador
// de pulsaciones y es enviado por el puerto serie.
// Utiliza polling y no filtra rebotes.
// José J. Lamas. UDC. 2019.

const int pulsador = 2;
int counter = 0;
 
void setup()
{
   Serial.begin(115200);
   pinMode(pulsador, INPUT);
   Serial.println("Start");
}
 
void loop() {
  leePulsador();
  delay(50);
}
 
void leePulsador() {
static boolean puls_enable= false;
  if (digitalRead(pulsador)) {
    if (puls_enable) {    
      puls_enable= false;
        Serial.print ("Count: ");
        Serial.println(++counter);
    }
  }
  else puls_enable= true;
}
