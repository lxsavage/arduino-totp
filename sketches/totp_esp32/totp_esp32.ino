/**
 * Arduino sketch for TOTP key generation
 * Written by Logan Savage
 */
#include <stdint.h>
#include <EEPROM.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>

#include <Base32-Decode.h>

#include "constants.hpp"
#include "totp.hpp"
#include "time.hpp"
#include "private_key.hpp"

static bool first_time;

static unsigned char decoded_key[TOTP_KEY_MAX];
static size_t decoded_len;

static hd44780_pinIO* lcd = nullptr;

void setup() {
  Serial.begin(BAUD_RATE);
  TOTP::init();
#ifdef ESP32
  EEPROM.begin(TOTP_KEY_MAX + OFFSET + 2);
#endif

  lcd = new hd44780_pinIO(RS, ENABLE, D4, D5, D6, D7);
  lcd->begin(16, 2);

  pinMode(SET_BTN, INPUT);
  if (digitalRead(SET_BTN)) {
    PrivateKey::set(lcd);
  }

  decoded_len = PrivateKey::load(lcd, decoded_key);
  if (decoded_len <= 0) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("ERROR");
    lcd->setCursor(0, 1);
    lcd->write("Key read failed!");
    lcd->print(decoded_len);
    Serial.printf("Key unset; len: %ld\n", decoded_len);
    for (;;) {
      delay(UINT32_MAX);
    }
  }

  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->write("Waiting for");
  lcd->setCursor(0, 1);
#ifdef ESP32
  lcd->write("NTP sync...");
#else
  lcd->write("UNIX time...");
  Serial.println("Load UNIX timestamp now...");
#endif
}

void loop() {
  if (Time::sync()) first_time = true;
  if (!Time::ready()) return;

  // Print a static text mask to prevent redrawing every letter every update
  if (first_time) {
    lcd->clear();
    lcd->setCursor(0, 1);
    lcd->write("Expires in   s");
  }
  
  long now = Time::get();

  static char code_buf[7];
  if (!TOTP::generate(decoded_key, decoded_len, now, code_buf)) {
    Serial.println("Failed to generate TOTP code");
    delay(POLL_MS);
    return;
  }

  lcd->setCursor(0, 0);
  for (int i = 0; i < 6; i++) {
    if (i == 3) lcd->write(' ');
    lcd->write(code_buf[i]);
  }

  static char exp_buf[3];
  snprintf(exp_buf, 3, "%02u", 30 - (now % 30));
  lcd->setCursor(11, 1);
  lcd->write(exp_buf);

  // Throttle polling
  delay(POLL_MS);
  first_time = false;
}
