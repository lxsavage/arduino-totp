#ifndef PRIVATE_KEY_HPP_INCLUDED
#define PRIVATE_KEY_HPP_INCLUDED

#include <stddef.h>

#include <hd44780ioClass/hd44780_pinIO.h>

#include "constants.hpp"

namespace PrivateKey {
  // Set the private key in storage
  void set(hd44780_pinIO* lcd);

  // Load the private key from storage
  size_t load(hd44780_pinIO* lcd, unsigned char out[TOTP_KEY_MAX]);
}

#endif