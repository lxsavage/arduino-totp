#include <Arduino.h>

#include "constants.hpp"
#include "storage.hpp"
#include "time.hpp"

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
  const char* ssid = "";
  const char* ppk = "";

  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);

  bool sync() {
    if (start_ts) return false;

    WiFi.disconnect();
    WiFi.begin(wifi.ssid, wifi.ppk);
    while (WiFi.status() != WL_CONNECTED) {
      if (
        WiFi.status() == WL_NO_SSID_AVAIL  ||
        WiFi.status() == WL_CONNECT_FAILED
      ) {
        return false;
      }

      delay(100);
    }

    timeClient.begin();
    timeClient.update();

    last_sync_millis = millis();
    start_ts = timeClient.getEpochTime();
    return true;
  }

  bool ready() {
    return start_ts != 0;
  }
  
  unsigned long get() {
    // Account for the number of seconds since the last sync
    return start_ts + ((millis() - last_sync_millis) / 1000);
  }
}