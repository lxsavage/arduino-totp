#ifndef TIME_HPP_INCLUDED
#define TIME_HPP_INCLUDED

namespace Time {
  // Get the current time
  long get();

  // Whether the time has been configured yet
  bool ready();

  // Use the best time reading method for the current platform
  bool sync();
}

#endif