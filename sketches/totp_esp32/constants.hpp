#ifndef CONSTANTS_HPP_INCLUDED
#define CONSTANTS_HPP_INCLUDED

#define BAUD_RATE 115200

// LCD to display codes/status messages
const int RS = 13;
const int ENABLE = 14;
const int D4 = 25;
const int D5 = 26;
const int D6 = 18;
const int D7 = 19;

// Hold this button when resetting to store a new base32 key
const int SET_BTN = 21;

// For debugging purposes; these don't need to be changed
const int TOTP_KEY_MAX = 65;
const int POLL_MS = 1000;

// Period for re-syncing time
const unsigned long TIME_SYNC_INTERVAL = 86400; // seconds

#endif
