#ifndef TOTP_H_INCLUDED
#define TOTP_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

namespace TOTP {
  // Initialize crypto libraries needed; run this again if the private key
  // changes
  void init();

  // Calculate a TOTP code for the given key and time
  // - key/key_len: the decoded private key and its character count. 
  // - time: the current unix timestamp, which is canonically a signed
  //         32-bit integer
  // - out: a buffer of at least 7 bytes to store the output/terminator in
  //
  // Returns true if successful, false otherwise.
  bool generate(uint8_t* key, size_t key_len, long time, char* out);
}

#endif