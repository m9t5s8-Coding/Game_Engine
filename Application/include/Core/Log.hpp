#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <mutex>
#include <vector>

#ifdef PLATFORM_ANDROID
  #include <android/log.h>
  #include <spdlog/sinks/android_sink.h>
#endif

#ifdef AERO_EDITOR
  #include <ImGui/ImGuiConsoleSink.hpp>
#endif

namespace ag {
// Log level configuration
struct LogConfig {
  spdlog::level::level_enum level                = spdlog::level::trace;
  bool                      enable_file_logging  = false;
  std::string               log_file_path        = "logs/aero.log";
  bool                      enable_console       = true;
  bool                      enable_imgui_console = false;
};

class Log {
public:
  // Initialize with default config
  static void init(const LogConfig& config = LogConfig());

  static void init_simple();

  static void init_android();

  static void set_level(spdlog::level::level_enum level);
  static void enable_file_logging(const std::string& path = "");
  static void disable_file_logging();

  // Logger accessors
  inline static std::shared_ptr<spdlog::logger>& get_core_logger() { return s_core_logger; }
  inline static std::shared_ptr<spdlog::logger>& get_client_logger() { return s_client_logger; }

#ifdef AERO_SERVER
  inline static std::shared_ptr<spdlog::logger>& get_server_logger() { return s_server_logger; }
#endif

#ifdef AERO_EDITOR
  inline static std::shared_ptr<ImGuiConsoleSink> get_console_sink() { return s_console_sink; }
  static void draw_console(const char* title = "Console", bool* p_open = nullptr);

  static void init_with_file();
  static void init_with_imGui();
#endif

  static void flush();

private:
  static void                          create_loggers(const LogConfig& config);
  static void                          setup_android_sink(std::vector<spdlog::sink_ptr>& sinks);
  static std::vector<spdlog::sink_ptr> create_sinks(const LogConfig& config);

#ifdef AERO_EDITOR
  static void setup_file_sink(std::vector<spdlog::sink_ptr>& sinks, const std::string& path);
  static void setup_console_sink(std::vector<spdlog::sink_ptr>& sinks);
  static void setup_imgui_sink(std::vector<spdlog::sink_ptr>& sinks);
#endif

  static std::shared_ptr<spdlog::logger> s_core_logger;
  static std::shared_ptr<spdlog::logger> s_client_logger;
#ifdef AERO_SERVER
  static std::shared_ptr<spdlog::logger> s_server_logger;
#endif
#ifdef AERO_EDITOR
  static std::shared_ptr<ImGuiConsoleSink> s_console_sink;
#endif
  static std::mutex s_mutex;
  static LogConfig  s_current_config;
};

}  // namespace ag

// Core logger macros
#define AERO_CORE_TRACE(...) ::ag::Log::get_core_logger()->trace(__VA_ARGS__)
#define AERO_CORE_DEBUG(...) ::ag::Log::get_core_logger()->debug(__VA_ARGS__)
#define AERO_CORE_INFO(...)  ::ag::Log::get_core_logger()->info(__VA_ARGS__)
#define AERO_CORE_WARN(...)  ::ag::Log::get_core_logger()->warn(__VA_ARGS__)
#define AERO_CORE_ERROR(...) ::ag::Log::get_core_logger()->error(__VA_ARGS__)
#define AERO_CORE_FATAL(...) ::ag::Log::get_core_logger()->critical(__VA_ARGS__)

// Client logger macros
#define AERO_TRACE(...) ::ag::Log::get_client_logger()->trace(__VA_ARGS__)
#define AERO_DEBUG(...) ::ag::Log::get_client_logger()->debug(__VA_ARGS__)
#define AERO_INFO(...)  ::ag::Log::get_client_logger()->info(__VA_ARGS__)
#define AERO_WARN(...)  ::ag::Log::get_client_logger()->warn(__VA_ARGS__)
#define AERO_ERROR(...) ::ag::Log::get_client_logger()->error(__VA_ARGS__)
#define AERO_FATAL(...) ::ag::Log::get_client_logger()->critical(__VA_ARGS__)

// Assertion macros with message
#define AERO_CORE_ASSERT(condition, ...)                                  \
  do {                                                                    \
    if (!(condition)) {                                                   \
      ::ag::Log::get_core_logger()->critical("Assertion failed: {} - {}", \
                                             #condition,                  \
                                             fmt::format(__VA_ARGS__));   \
    }                                                                     \
  } while (0)

#define AERO_ASSERT(condition, ...)                                         \
  do {                                                                      \
    if (!(condition)) {                                                     \
      ::ag::Log::get_client_logger()->critical("Assertion failed: {} - {}", \
                                               #condition,                  \
                                               fmt::format(__VA_ARGS__));   \
    }                                                                       \
  } while (0)

// Simple assertion without extra message
#define AERO_CORE_ASSERT_SIMPLE(condition)                 \
  do {                                                     \
    if (!(condition)) {                                    \
      AERO_CORE_FATAL("Assertion failed: {}", #condition); \
      assert(condition);                                   \
    }                                                      \
  } while (0)

#define AERO_ASSERT_SIMPLE(condition)                 \
  do {                                                \
    if (!(condition)) {                               \
      AERO_FATAL("Assertion failed: {}", #condition); \
      assert(condition);                              \
    }                                                 \
  } while (0)

#ifdef AERO_SERVER
   // Server logger macros
  #define AERO_SERVER_TRACE(...) ::ag::Log::get_server_logger()->trace(__VA_ARGS__)
  #define AERO_SERVER_DEBUG(...) ::ag::Log::get_server_logger()->debug(__VA_ARGS__)
  #define AERO_SERVER_INFO(...)  ::ag::Log::get_server_logger()->info(__VA_ARGS__)
  #define AERO_SERVER_WARN(...)  ::ag::Log::get_server_logger()->warn(__VA_ARGS__)
  #define AERO_SERVER_ERROR(...) ::ag::Log::get_server_logger()->error(__VA_ARGS__)
  #define AERO_SERVER_FATAL(...) ::ag::Log::get_server_logger()->critical(__VA_ARGS__)

  #define AERO_SERVER_ASSERT(condition, ...)                                  \
    do {                                                                      \
      if (!(condition)) {                                                     \
        ::ag::Log::get_server_logger()->critical("Assertion failed: {} - {}", \
                                                 #condition,                  \
                                                 fmt::format(__VA_ARGS__));   \
      }                                                                       \
    } while (0)
#endif

// Performance timing macro
#define AERO_PROFILE_SCOPE(name)                                                                  \
  struct ProfileData_##__LINE__ {                                                                 \
    ProfileData_##__LINE__()                                                                      \
      : start(std::chrono::high_resolution_clock::now()) {}                                       \
    ~ProfileData_##__LINE__() {                                                                   \
      auto end      = std::chrono::high_resolution_clock::now();                                  \
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
      AERO_TRACE("Profile: {} took {} μs", name, duration);                                       \
    }                                                                                             \
    std::chrono::time_point<std::chrono::high_resolution_clock> start;                            \
  } profile_##__LINE__

//-----------------------------------------------------------------------------
// Implementation (typically in a .cpp file)
//-----------------------------------------------------------------------------

#ifdef PLATFORM_ANDROID
   // Android-specific convenience macro for logcat
  #define AERO_ANDROID_LOGI(...) __android_log_print(ANDROID_LOG_INFO, "AERO", __VA_ARGS__)
  #define AERO_ANDROID_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "AERO", __VA_ARGS__)
#endif
