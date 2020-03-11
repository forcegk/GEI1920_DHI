// Icons display LCD Monkey Traps
// J.J.Lamas. UDC. 2019.

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const uint8_t monkey[8] = {
        B01110,
        B01110,
        B00100,
        B01110,
        B10101,
        B00100,
        B01010,
        B10001
};
const uint8_t mon_trapped[8] = {
        B10001,
        B10001,
        B11011,
        B10001,
        B01010,
        B11011,
        B10101,
        B01110
};
const uint8_t trap[8] = {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111
};
const uint8_t tiger[8] = {
        B10001,
        B01110,
        B10001,
        B10101,
        B00100,
        B10001,
        B01010,
        B00100
};
const uint8_t bananas[8] = {
        B10101,
        B10101,
        B10101,
        B00000,
        B10101,
        B10101,
        B10101,
        B00000
};

void setup() {
  analogWrite(6,60);     // LCD contrast voltage to pin 6
  lcd.createChar(1, monkey);
  lcd.createChar(2, trap);
  lcd.createChar(3, tiger);
  lcd.createChar(4, bananas);
  lcd.createChar(5, mon_trapped);
  lcd.begin(16, 2);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void loop() {}
