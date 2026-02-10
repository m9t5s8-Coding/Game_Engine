#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

#include <Core/Time.hpp>
#include <cstdint>

#ifdef PLATFORM_WINDOWS
// Windows implementation using QueryPerformanceCounter
#include <windows.h>

namespace ag
{
  static const LARGE_INTEGER& get_qpc_frequency()
  {
    static LARGE_INTEGER frequency = []()
    {
      LARGE_INTEGER freq;
      QueryPerformanceFrequency(&freq);
      return freq;
    }();
    return frequency;
  }

  double Time::get_time()
  {
    static double start_time = []()
    {
      LARGE_INTEGER counter;
      QueryPerformanceCounter(&counter);
      return static_cast<double>(counter.QuadPart) /
             static_cast<double>(get_qpc_frequency().QuadPart);
    }();

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    double current_time = static_cast<double>(counter.QuadPart) /
                          static_cast<double>(get_qpc_frequency().QuadPart);
    return current_time - start_time;
  }

  uint64_t Time::get_ticks()
  {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
  }

  double Time::get_frequency()
  {
    return static_cast<double>(get_qpc_frequency().QuadPart);
  }
}

#elif defined(PLATFORM_LINUX)
// Linux implementation using clock_gettime (POSIX)
#include <time.h>

namespace ag
{
  double Time::get_time()
  {
    static double start_time = []()
    {
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC, &ts);
      return static_cast<double>(ts.tv_sec) +
             static_cast<double>(ts.tv_nsec) * 1e-9;
    }();

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    double current_time = static_cast<double>(ts.tv_sec) +
                          static_cast<double>(ts.tv_nsec) * 1e-9;
    return current_time - start_time;
  }

  uint64_t Time::get_ticks()
  {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // Return nanoseconds as ticks
    return static_cast<uint64_t>(ts.tv_sec) * 1000000000ULL +
           static_cast<uint64_t>(ts.tv_nsec);
  }

  double Time::get_frequency()
  {
    // CLOCK_MONOTONIC operates in nanoseconds
    return 1000000000.0; // 1 GHz (nanosecond precision)
  }
}

#endif // PLATFORM_LINUX

#endif // PLATFORM_WINDOWS || PLATFORM_LINUX
