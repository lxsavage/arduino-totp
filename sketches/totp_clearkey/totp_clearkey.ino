/**
 * Arduino sketch for securely clearing TOTP private key from EEPROM
 * Written by Logan Savage
 */
#include <EEPROM.h>

// Memory offset for writing to EEPROM
#define OFFSET 0

uint16_t key_len(uint16_t offset) {
  return EEPROM.read(1 + offset) << 8 | EEPROM.read(0 + offset);
}

void clear_key(uint16_t len, uint16_t offset) {
  EEPROM.write(offset, 0);
  EEPROM.write(1 + offset, 0);
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i + offset, 0);
  }
}

long hardware_random(long min, long max) {
  static bool firsttime = true;
  static unsigned long rand_seed = micros();

  if (firsttime) {
    firsttime = false;
    randomSeed(rand_seed + analogRead(A0));
    if (!analogRead(A0))
      Serial.println("WARNING: not using analog input, which may result in deterministic clear. Add a random source to A0 pin!");
  }
  return random(min, max);
}


void setup() {
  Serial.begin(9600);
  uint16_t len = key_len(OFFSET);
  Serial.print("Key is ");
  Serial.print(len);
  Serial.println(" bytes.");

  if (len > 0) {
    Serial.println("Scrambling key...");
    for (uint16_t i = 0; i < len; i++) {
      if (hardware_random(0, 2) == 0) continue;
      EEPROM.write(OFFSET + 2 + i, (uint8_t)hardware_random(0, 256));
    }
    EEPROM.write(OFFSET, (uint16_t)0);
  }
  Serial.println("Done!");
}


void loop() {
}
