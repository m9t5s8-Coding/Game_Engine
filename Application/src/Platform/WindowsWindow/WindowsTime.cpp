#ifdef PLATFORM_WINDOWS

#include <Core/Time.hpp>
#include <windows.h>
#include <cstdint>


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
#endif