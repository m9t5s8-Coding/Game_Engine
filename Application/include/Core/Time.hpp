#pragma once

#include <cstdint>

namespace ag {
class Time {
public:
  static double   get_time();
  static uint64_t get_ticks();
  static double   get_frequency();
};
}  // namespace ag
