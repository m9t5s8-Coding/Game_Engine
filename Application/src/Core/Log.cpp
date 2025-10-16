#include <Core/Log.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ag
{
  std::shared_ptr<spdlog::logger> Log::s_core_logger;
  std::shared_ptr<spdlog::logger> Log::s_client_logger;

  void Log::init()
  {
    //this is the pattern of the log time name and message
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_core_logger = spdlog::stdout_color_mt("AERO");
    s_core_logger->set_level(spdlog::level::trace);
    s_client_logger = spdlog::stdout_color_mt("APP");
    s_client_logger->set_level(spdlog::level::trace);
  }
}
