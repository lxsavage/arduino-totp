#include "private_key.hpp"
#include <stddef.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <Base32-Decode.h>

extern HardwareSerial Serial;

namespace PrivateKey {
  // Normalize Base32 secret: remove spaces, uppercase, add padding
  String normalizeBase32Secret(const char* input) {
    String secret = input;
    // Remove spaces and convert to uppercase
    secret.replace(" ", "");
    secret.toUpperCase();
    
    // Add padding if needed (must be multiple of 8)
    int len = secret.length();
    int paddingNeeded = (8 - (len % 8)) % 8;
    for (int i = 0; i < paddingNeeded; i++) {
      secret += '=';
    }
    return secret;
  }

  void set(hd44780_pinIO* lcd) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Waiting for key...");
    
    while (!Serial.available()) {
      delay(10);
    }

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Reading key...");

    char key_buf[255];
    size_t key_len = Serial.readBytesUntil('\n', key_buf, 254);
    key_buf[key_len] = '\0';

    // Normalize Base32 secret (critical for QR code secrets)
    String normalized = normalizeBase32Secret(key_buf);
    
    unsigned char decoded[TOTP_KEY_MAX];
    size_t decoded_len = base32decode(normalized.c_str(), decoded, TOTP_KEY_MAX);
    
    if (decoded_len <= 0 || decoded_len > TOTP_KEY_MAX) {
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->write("Decode failed!");
      lcd->setCursor(0, 1);
      lcd->print("Len: ");
      lcd->print(decoded_len);
      Serial.printf("Decode failed; len: %d\n", decoded_len);
      delay(2000);
      return;
    }

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Saving to EEPROM...");

    // Store length as uint16_t in big-endian format
    EEPROM.write(OFFSET, (decoded_len >> 8) & 0xFF);
    EEPROM.write(OFFSET + 1, decoded_len & 0xFF);
    
    // Store key bytes
    for (size_t i = 0; i < decoded_len; i++) {
      EEPROM.write(OFFSET + i + 2, decoded[i]);
    }
    
  #ifdef ESP32
    if (!EEPROM.commit()) {
      Serial.println("EEPROM commit FAILED!");
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->write("EEPROM ERROR!");
      delay(2000);
      return;
    }
    Serial.println("EEPROM commit successful");
  #endif

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Key saved!");
    delay(1000);
  }

  size_t load(hd44780_pinIO* lcd, unsigned char out[TOTP_KEY_MAX]) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->write("Loading key...");

    // Read length header directly (2 bytes, big-endian)
    uint16_t len = (EEPROM.read(OFFSET) << 8) | EEPROM.read(OFFSET + 1);
    
    // Validate length
    if (len == 0 || len > TOTP_KEY_MAX) {
      Serial.printf("Invalid key length: %d (OFFSET=%d)\n", len, OFFSET);
      return 0;
    }

    // Read key bytes
    for (size_t i = 0; i < len; i++) {
      out[i] = EEPROM.read(OFFSET + i + 2);
    }

    return len;
  }
}