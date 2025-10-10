#pragma once


#include <spdlog/spdlog.h>
#include <memory>

namespace aero
{
  class Log
  {
  public:
    static void init();

    inline static std::shared_ptr<spdlog::logger> &get_core_logger() { return s_core_logger; }
    inline static std::shared_ptr<spdlog::logger> &get_client_logger() { return s_client_logger; }

  private:
    static std::shared_ptr<spdlog::logger> s_core_logger;
    static std::shared_ptr<spdlog::logger> s_client_logger;
  };
}

//Core log macro
#define AERO_CORE_ERROR(...) ::aero::Log::get_core_logger()->error(__VA_ARGS__)
#define AERO_CORE_WARN(...) ::aero::Log::get_core_logger()->warn(__VA_ARGS__)
#define AERO_CORE_INFO(...) ::aero::Log::get_core_logger()->info(__VA_ARGS__)
#define AERO_CORE_TRACE(...) ::aero::Log::get_core_logger()->trace(__VA_ARGS__)
#define AERO_CORE_FATAL(...) ::aero::Log::get_core_logger()->fatal(__VA_ARGS__)
#define AERO_CORE_ASSERT(x,...) if(!x) ::aero::Log::get_core_logger()->info(__VA_ARGS__)

// Client log macro
#define AERO_ERROR(...) ::aero::Log::get_client_logger()->error(__VA_ARGS__)
#define AERO_WARN(...) ::aero::Log::get_client_logger()->warn(__VA_ARGS__)
#define AERO_INFO(...) ::aero::Log::get_client_logger()->info(__VA_ARGS__)
#define AERO_TRACE(...) ::aero::Log::get_client_logger()->trace(__VA_ARGS__)
#define AERO_FATAL(...) ::aero::Log::get_client_logger()->fatal(__VA_ARGS__)
#define AERO_ASSERT(x,...) if(!x) ::aero::Log::get_client_logger()->info(__VA_ARGS__)



