#include <LiquidCrystal_I2C.h>

const int DATA_PIN = 8;
const int LED_PIN  = 13;
const int BIT_MS   = 300;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const uint8_t START_MSG = 30; // 11110
const uint8_t END_MSG   = 31; // 11111

const char message[] = "HELLO-WORLD";

uint8_t encode5(char c) {
  if (c >= 'a' && c <= 'z') c -= 32;
  if (c >= 'A' && c <= 'Z') return (uint8_t)(c - 'A');
  switch (c) {
    case '-': return 26;
    case '\'': return 27;
    case '.': return 28;
    case '/': return 29;
    default:  return 255;
  }
}

void showWholeMessage(const char* msg) {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 16; i++) {
    char c = msg[i];
    lcd.print(c ? c : ' ');
    if (!c) {
      for (int j = i + 1; j < 16; j++) lcd.print(' ');
      break;
    }
  }
}

void clearBitRow() {
  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) lcd.print(' ');
}

void putBitOnRow2(int pos, bool bitVal) {
  lcd.setCursor(pos, 1);
  lcd.print(bitVal ? '1' : '0');
}

void sendBit(bool v) {
  digitalWrite(DATA_PIN, v ? HIGH : LOW);
  digitalWrite(LED_PIN,  v ? HIGH : LOW);
  digitalWrite(2,HIGH);
  delay(BIT_MS);
  digitalWrite(2, LOW);
}

void sendCodeAnimated(uint8_t code) {
  clearBitRow();

  sendBit(true); // start bit HIGH

  for (int b = 4; b >= 0; --b) {
    bool bitVal = (code >> b) & 1;
    putBitOnRow2(4 - b, bitVal);
    sendBit(bitVal);
  }

  sendBit(false); // stop bit LOW
}

void sendCharAtIndex(char c, int i) {
  uint8_t code = encode5(c);
  if (code == 255) return;

  // underline the current letter with the LCD cursor
  lcd.setCursor(i, 0);
  lcd.cursor();     // underline cursor
  lcd.noBlink();    // steady underline

  sendCodeAnimated(code);
}

void sendMessage(const char* msg) {
  showWholeMessage(msg);

  // send start-of-message (cursor hidden)
  lcd.noCursor();
  sendCodeAnimated(START_MSG);

  //send chars
  for (int i = 0; msg[i] != '\0' && i < 16; i++) {
    sendCharAtIndex(msg[i], i);
  }

  // send end-of-message (cursor hidden)
  lcd.noCursor();
  sendCodeAnimated(END_MSG);

  lcd.noCursor();
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  sendMessage(message);
  delay(500);
}


