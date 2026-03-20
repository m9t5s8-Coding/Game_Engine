#pragma once
#include <spdlog/spdlog.h>

#include <functional>
#include <ImGui/ImGuiConsoleSink.hpp>
#include <memory>
#include <vector>

namespace ag
{
class Log
{
public:
  static void init();

  static void init_with_console();

  inline static std::shared_ptr<spdlog::logger>& get_core_logger()
  {
    return s_core_logger;
  }
  inline static std::shared_ptr<spdlog::logger>& get_client_logger()
  {
    return s_client_logger;
  }

#ifdef AERO_SERVER
  static void                                    init_server();
  inline static std::shared_ptr<spdlog::logger>& get_server_logger()
  {
    return s_server_logger;
  }
#endif

  static void add_to_console(const std::string&        message,
                             spdlog::level::level_enum level,
                             const char*               logger_name);

  static void draw_console(const char* title = "Console", bool* p_open = nullptr);

  inline static std::shared_ptr<ImGuiConsoleSink> get_console_sink()
  {
    return s_console_sink;
  }

private:
  static std::shared_ptr<spdlog::logger> s_core_logger;
  static std::shared_ptr<spdlog::logger> s_app_logger;
  static std::shared_ptr<spdlog::logger> s_client_logger;

#ifdef AERO_SERVER
  static std::shared_ptr<spdlog::logger> s_server_logger;
#endif
  static std::shared_ptr<ImGuiConsoleSink> s_console_sink;
};
}  // namespace ag

#define AERO_CORE_ERROR(...)                                                                       \
  {                                                                                                \
    ::ag::Log::get_core_logger()->error(__VA_ARGS__);                                              \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::err, "AERO");               \
  }

#define AERO_CORE_WARN(...)                                                                        \
  {                                                                                                \
    ::ag::Log::get_core_logger()->warn(__VA_ARGS__);                                               \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::warn, "AERO");              \
  }

#define AERO_CORE_INFO(...)                                                                        \
  {                                                                                                \
    ::ag::Log::get_core_logger()->info(__VA_ARGS__);                                               \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::info, "AERO");              \
  }

#define AERO_CORE_TRACE(...)                                                                       \
  {                                                                                                \
    ::ag::Log::get_core_logger()->trace(__VA_ARGS__);                                              \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::trace, "AERO");             \
  }

#define AERO_CORE_FATAL(...)                                                                       \
  {                                                                                                \
    ::ag::Log::get_core_logger()->critical(__VA_ARGS__);                                           \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::critical, "AERO");          \
  }

#define AERO_CORE_ASSERT(x, ...)                                                                   \
  if (!(x))                                                                                        \
  {                                                                                                \
    ::ag::Log::get_core_logger()->critical(__VA_ARGS__);                                           \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::critical, "AERO");          \
  }

// Client log macros - FIXED: Now pass logger name correctly
#define AERO_ERROR(...)                                                                            \
  {                                                                                                \
    ::ag::Log::get_client_logger()->error(__VA_ARGS__);                                            \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::err, "APP");                \
  }

#define AERO_WARN(...)                                                                             \
  {                                                                                                \
    ::ag::Log::get_client_logger()->warn(__VA_ARGS__);                                             \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::warn, "APP");               \
  }

#define AERO_INFO(...)                                                                             \
  {                                                                                                \
    ::ag::Log::get_client_logger()->info(__VA_ARGS__);                                             \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::info, "APP");               \
  }

#define AERO_TRACE(...)                                                                            \
  {                                                                                                \
    ::ag::Log::get_client_logger()->trace(__VA_ARGS__);                                            \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::trace, "APP");              \
  }

#define AERO_FATAL(...)                                                                            \
  {                                                                                                \
    ::ag::Log::get_client_logger()->critical(__VA_ARGS__);                                         \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::critical, "APP");           \
  }

#define AERO_ASSERT(x, ...)                                                                        \
  if (!(x))                                                                                        \
  {                                                                                                \
    ::ag::Log::get_client_logger()->critical(__VA_ARGS__);                                         \
    ::ag::Log::add_to_console(fmt::format(__VA_ARGS__), spdlog::level::critical, "APP");           \
  }

#ifdef AERO_SERVER
  #define AERO_SERVER_ERROR(...)                                                                   \
    {                                                                                              \
      ::ag::Log::get_server_logger()->error(__VA_ARGS__);                                          \
    }

  #define AERO_SERVER_WARN(...)                                                                    \
    {                                                                                              \
      ::ag::Log::get_server_logger()->warn(__VA_ARGS__);                                           \
    }

  #define AERO_SERVER_INFO(...)                                                                    \
    {                                                                                              \
      ::ag::Log::get_server_logger()->info(__VA_ARGS__);                                           \
    }

  #define AERO_SERVER_TRACE(...)                                                                   \
    {                                                                                              \
      ::ag::Log::get_server_logger()->trace(__VA_ARGS__);                                          \
    }

  #define AERO_SERVER_FATAL(...)                                                                   \
    {                                                                                              \
      ::ag::Log::get_server_logger()->critical(__VA_ARGS__);                                       \
    }

  #define AERO_SERVER_ASSERT(x, ...)                                                               \
    if (!(x))                                                                                      \
    {                                                                                              \
      ::ag::Log::get_server_logger()->critical(__VA_ARGS__);                                       \
    }

#endif
