
#ifdef PLATFORM_ANDROID

  #include <time.h>

  #include <Core/Time.hpp>

namespace ag
{
double Time::get_time()
{
  static double start_time = []()
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) * 1e-9;
  }();

  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  double current_time = static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) * 1e-9;
  return current_time - start_time;
}

uint64_t Time::get_ticks()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  // Return nanoseconds as ticks
  return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL + static_cast<uint64_t>(ts.tv_nsec);
}

double Time::get_frequency()
{
  // CLOCK_MONOTONIC operates in nanoseconds
  return 1000000000.0;  // 1 GHz (nanosecond precision)
}
}  // namespace ag

#endif  // PLATFORM_WINDOWS || PLATFORM_LINUX
