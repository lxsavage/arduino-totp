#ifndef CONSTANTS_HPP_INCLUDED
#define CONSTANTS_HPP_INCLUDED

#define BAUD_RATE 115200

#ifdef ESP32
  // LCD to display codes/status messages
  const int RS = 13;
  const int ENABLE = 14;
  const int D4 = 25;
  const int D5 = 26;
  const int D6 = 18;
  const int D7 = 19;

  // Hold this button when resetting to store a new base32 key
  const int SET_BTN = 21;
#else
  // LCD to display codes/status messages
  const int RS = 12;
  const int ENABLE = 11;
  const int D4 = 5;
  const int D5 = 4;
  const int D6 = 3;
  const int D7 = 2;

  // Hold this button when resetting to store a new base32 key
  const int SET_BTN = 8;
#endif

// Memory offset for writing to EEPROM/flash
const int OFFSET = 0;

// For debugging purposes; these don't need to be changed
const int TOTP_KEY_MAX = 64;
const int POLL_MS = 1000;

#endif
