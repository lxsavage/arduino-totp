#include <Arduino.h>

extern HardwareSerial Serial;

#include "time.h"

#ifdef ESP32
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#endif

namespace Time {
  long start_ts;

  long get() {
    return start_ts + (millis() / 1000);
  }

  bool ready() {
    return start_ts != 0;
  }

#ifdef ESP32
  const char* ssid = "The Password Is Gullible";
  const char* ppk = "2444666668888888000000000";

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);

  bool sync() {
    if (start_ts) return false;

    WiFi.disconnect();
    WiFi.begin(ssid, ppk);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }

    timeClient.begin();
    timeClient.setTimeOffset(0);
    timeClient.update();
    start_ts = timeClient.getEpochTime();
    return true;
  }
#else
  bool sync() {
    if (!Serial.available()) return false;
    long preread_offset = millis() / 1000;
    long ts = Serial.parseInt();
    if (ts != 0)
      start_ts = ts - preread_offset;
    return true;
  }
#endif
}
