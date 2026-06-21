#include <Base32-Decode.h>
#include <TOTP.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD
#define RS     12
#define ENABLE 11
#define D4     5
#define D5     4
#define D6     3
#define D7     2

// Hold this button when resetting to store a new base32 key
#define SET_BTN 8

#define TOTP_KEY_MAX 64
#define POLL_MS 1000

// Flag signifying the first time the loop executes after updating the timestamp
bool first_time;

// Base32-encoded TOTP private key and decoded storage
unsigned char decoded_key[TOTP_KEY_MAX];
size_t decoded_len;

LiquidCrystal lcd(RS, ENABLE, D4, D5, D6, D7);
TOTP* totp = nullptr;

// the Unix timestamp when the program started
unsigned long start_ts;
unsigned long time() {
  return start_ts + (millis() / 1000);
}

// For use with unixsync
void sync_time() {
  unsigned long preread_offset = millis() / 1000;
  unsigned long ts = Serial.parseInt();
  if (ts != 0) start_ts = ts - preread_offset;
}

void set_key() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Waiting for");
  lcd.setCursor(0, 1);
  lcd.write("Private key...");
  // Serial.println("Load private key now...");
  while (!Serial.available()) {}

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Decoding key...");
  unsigned char key_buf[255] = {'\0'};
  size_t key_len = Serial.readBytesUntil('\0', key_buf, 255);

  char decoded[TOTP_KEY_MAX];
  size_t decoded_len = base32decode(
    key_buf,
    decoded,
    TOTP_KEY_MAX
  );
  if (decoded_len < 0) {
    Serial.println("failed to decode key");
    lcd.setCursor(0, 0);
    lcd.write("Failed to read key!");
    delay(1000);
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Writing key...");
  // Serial.println("Writing key to EEPROM");

  EEPROM.write(0, (uint16_t)decoded_len);
  for (int i = 0; i < decoded_len; i++) {
    EEPROM.write(i+2, decoded[i]);
  }

  // Serial.print(decoded_len);
  // Serial.println(" bytes");
}

size_t load_key(unsigned char out[TOTP_KEY_MAX]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write("Retrieving key...");
  unsigned char r;
  EEPROM.get(0, r);
  if (r == 0) return;

  // Convert EEPROM len value from big endian to little endian
  uint16_t len = EEPROM.read(1) << 8 | EEPROM.read(0);
  if (len == 0) return 0;

  for (size_t i = 0; i < len; i++) {
    out[i] = EEPROM.read(i+2);
  }
  return (size_t)len;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(SET_BTN, INPUT);
  if (digitalRead(SET_BTN)) {
    set_key();
  }
  decoded_len = load_key(decoded_key);

  if (decoded_len == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write("ERROR");
    lcd.setCursor(0, 1);
    lcd.write("Key not set!");
    for(;;);
  }

  totp = new TOTP(decoded_key, decoded_len);
  // Serial.println("Load UNIX timestamp now...");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for");
  lcd.setCursor(0, 1);
  lcd.print("UNIX time...");
}

void loop() {
  if (Serial.available()) {
    sync_time();
    first_time = true;
  }
  if (!start_ts) return;

  // Print a static text mask to prevent redrawing every letter every update
  if (first_time) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Expires in   s");
  }

  static long last_change = 0;
  static char* code = nullptr;

  long now = time();
  long current_T = now / 30;
  bool code_change = code == nullptr || first_time || current_T != last_change;
  if (code_change) {
    code = totp->getCode(now);
    last_change = current_T;

    // Print out the TOTP code with a space separating the first and last 3 digits
    lcd.setCursor(0, 0);
    for (int i = 0; i < 6; i++) {
      if (i == 3) lcd.write(' ');
      lcd.write(code[i]);
    }
  }

  // Write the expiration time
  char expiration_buf[3];
  snprintf(expiration_buf, 3, "%02u", 30 - (now % 30));
  lcd.setCursor(11, 1);
  lcd.print(expiration_buf);

  // Throttle polling
  delay(POLL_MS);
  first_time = false;
}
