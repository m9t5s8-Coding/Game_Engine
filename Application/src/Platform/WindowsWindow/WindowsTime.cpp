#ifdef PLATFORM_WINDOWS
#include <Core/Time.hpp>
#include <windows.h>

namespace ag
{
	double Time::get_time()
	{
    static LARGE_INTEGER frequency;
    static BOOL qpcAvailable = QueryPerformanceFrequency(&frequency);

    if (qpcAvailable)
    {
      LARGE_INTEGER counter;
      QueryPerformanceCounter(&counter);
      return static_cast<double>(counter.QuadPart) / static_cast<double>(frequency.QuadPart);
    }
    else
    {
      return static_cast<double>(GetTickCount64()) / 1000.0;
    }
	}

  uint64_t Time::get_ticks()
  {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
  }

  double Time::get_frequency()
  {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return static_cast<double>(frequency.QuadPart);
  }
}


#endif