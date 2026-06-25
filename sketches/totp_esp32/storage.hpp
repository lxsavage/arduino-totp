#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <stddef.h>

#include "constants.hpp"

namespace storage {
  struct PrivateKey {
    size_t len;
    unsigned char key[TOTP_KEY_MAX];
  };
  
  struct NetworkData {
    char ppk[64];
    char ssid[32];
  };

  void init();

  void load_privatekey(struct PrivateKey& out);
  void load_wifi(struct NetworkData& out);

  void write_privatekey(struct PrivateKey& in);
  void write_wifi(struct NetworkData& in);

  bool commit_writes();
}

#endif