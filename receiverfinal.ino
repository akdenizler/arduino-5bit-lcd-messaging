#include <LiquidCrystal_I2C.h>

const int DATA_PIN = 8;
const int LED_PIN  = 13;
const int BIT_MS   = 300;

const uint8_t START_MSG = 30; // 11110
const uint8_t END_MSG   = 31; // 11111

LiquidCrystal_I2C lcd(0x27, 16, 2);

char decode5(uint8_t code) {
  code &= 0x1F;
  if (code <= 25) return (char)('A' + code);
  switch (code) {
    case 26: return '-';
    case 27: return '\'';
    case 28: return '.';
    case 29: return '/';
    default: return '?';
  }
}

void clearTopRow() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) lcd.print(' ');
}

void clearBitRow() {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) lcd.print(' ');
}

void putBitOnRow2(int pos, bool bitVal) {
  lcd.setCursor(pos, 1);
  lcd.print(bitVal ? '1' : '0');
}

//read one framed 5-bit code: start(1), 5 bits, stop(0)
//shows the 5 bits appearing on row 2 as they arrive.
//returns 255 if framing error.
uint8_t readCodeAnimated() {
  // wait for start bit HIGH
  while (digitalRead(DATA_PIN) == LOW) {}

  // move into the first data-bit window
  delay(BIT_MS);

  clearBitRow();

  uint8_t code = 0;

  // read 5 bits MSB -> LSB
  for (int b = 4; b >= 0; --b) {
    int v = digitalRead(DATA_PIN);
    digitalWrite(LED_PIN, v ? HIGH : LOW);

    putBitOnRow2(4 - b, v);

    if (v) code |= (1 << b);
    delay(BIT_MS);
  }

  // stop bit (should be LOW)
  int stopBit = digitalRead(DATA_PIN);
  digitalWrite(LED_PIN, stopBit ? HIGH : LOW);
  delay(BIT_MS);

  if (stopBit != LOW) return 255; // framing error
  return code & 0x1F;
}

void setup() {
  pinMode(DATA_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  static bool inMessage = false;
  static int  outPos = 0;

  uint8_t code = readCodeAnimated();
  if (code == 255) return; // ignore bad frame

  if (code == START_MSG) {
    inMessage = true;
    outPos = 0;
    clearTopRow();
    clearBitRow();
    return;
  }

  if (!inMessage) return;

  if (code == END_MSG) {
    inMessage = false;
    digitalWrite(6, HIGH);
    delay(500);
    digitalWrite(6, LOW);
    clearBitRow(); // message done; keep top row as final message
  
    return;
  }

  // normal character
  if (outPos < 16) {
    char c = decode5(code);
    lcd.setCursor(outPos, 0);
    lcd.print(c);
    outPos++;
  }
}

