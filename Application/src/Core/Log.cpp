#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifdef PLATFORM_ANDROID
  #include <spdlog/sinks/android_sink.h>
#endif

#include <sys/stat.h>

#include <chrono>
#include <Core/Log.hpp>
#include <iostream>

namespace ag
{

std::shared_ptr<spdlog::logger> Log::s_core_logger;
std::shared_ptr<spdlog::logger> Log::s_client_logger;
#ifdef AERO_SERVER
std::shared_ptr<spdlog::logger> Log::s_server_logger;
#endif
#ifdef AERO_EDITOR
std::shared_ptr<ImGuiConsoleSink> Log::s_console_sink;
#endif
std::mutex Log::s_mutex;
LogConfig  Log::s_current_config;

void Log::init(const LogConfig& config)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  s_current_config = config;
  create_loggers(config);
}

void Log::init_simple()
{
  LogConfig config;
  config.enable_console       = true;
  config.enable_file_logging  = false;
  config.enable_imgui_console = false;
  init(config);
}

#ifdef AERO_EDITOR
void Log::init_with_file()
{
  LogConfig config;
  config.enable_console       = true;
  config.enable_file_logging  = true;
  config.enable_imgui_console = false;
  init(config);
}

void Log::init_with_imGui()
{
  s_console_sink = std::make_shared<ImGuiConsoleSink>(5000);

  std::vector<spdlog::sink_ptr> sinks;

  #ifndef PLATFORM_ANDROID
  // Console sink for desktop
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_pattern("%^[%T.%e] [%n] [%l] %v%$");
  sinks.push_back(console_sink);
  #else
  // Android logcat sink
  auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>("AERO");
  android_sink->set_pattern("[%n] [%l] %v");
  sinks.push_back(android_sink);
  #endif

  // Add ImGui console sink
  sinks.push_back(s_console_sink);

  // Optional file logging
  if (s_current_config.enable_file_logging)
  {
    try
    {
      auto file_sink =
          std::make_shared<spdlog::sinks::basic_file_sink_mt>(s_current_config.log_file_path, true);
      file_sink->set_pattern("[%Y-%m-%d %T.%e] [%n] [%l] %v");
      sinks.push_back(file_sink);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
      std::cerr << "Log file creation failed: " << ex.what() << std::endl;
    }
  }

  // Create loggers
  s_core_logger = std::make_shared<spdlog::logger>("CORE", sinks.begin(), sinks.end());
  s_core_logger->set_level(s_current_config.level);
  s_core_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");

  s_client_logger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
  s_client_logger->set_level(s_current_config.level);
  s_client_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");

  #ifdef AERO_SERVER
  s_server_logger = std::make_shared<spdlog::logger>("SERVER", sinks.begin(), sinks.end());
  s_server_logger->set_level(s_current_config.level);
  s_server_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");
  #endif

  spdlog::set_default_logger(s_client_logger);
  spdlog::flush_on(spdlog::level::err);
}

void Log::setup_console_sink(std::vector<spdlog::sink_ptr>& sinks)
{
  if (!s_console_sink)
    s_console_sink = std::make_shared<ImGuiConsoleSink>();
  sinks.push_back(s_console_sink);
}
void Log::setup_imgui_sink(std::vector<spdlog::sink_ptr>& sinks)
{
  if (!s_console_sink)
  {
    s_console_sink = std::make_shared<ImGuiConsoleSink>();
  }
  s_console_sink->set_pattern("[%T.%e] [%n] [%l] %v");
  sinks.push_back(s_console_sink);
}

void Log::draw_console(const char* title, bool* p_open)
{
  if (s_console_sink)
  {
    s_console_sink->Draw(title, p_open);  // Note: Draw with capital D to match our implementation
  }
}

void Log::setup_file_sink(std::vector<spdlog::sink_ptr>& sinks, const std::string& path)
{
  try
  {
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true);
    file_sink->set_pattern("[%Y-%m-%d %T.%e] [%n] [%l] %v");
    sinks.push_back(file_sink);
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    std::cerr << "Log file creation failed: " << ex.what() << std::endl;
  }
}
#endif

void Log::init_android()
{
  LogConfig config;
  config.enable_console       = false;  // Use Android logcat instead
  config.enable_file_logging  = true;   // Optionally also log to file
  config.enable_imgui_console = false;
  init(config);
}

void Log::create_loggers(const LogConfig& config)
{
  auto sinks = create_sinks(config);

  // Create core logger
  s_core_logger = std::make_shared<spdlog::logger>("CORE", sinks.begin(), sinks.end());
  s_core_logger->set_level(config.level);
  s_core_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");

  // Create client logger
  s_client_logger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
  s_client_logger->set_level(config.level);
  s_client_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");

#ifdef AERO_SERVER
  s_server_logger = std::make_shared<spdlog::logger>("SERVER", sinks.begin(), sinks.end());
  s_server_logger->set_level(config.level);
  s_server_logger->set_pattern("%^[%T.%e] [%n] [%l] %v%$");
#endif

  // Set as default loggers
  spdlog::set_default_logger(s_client_logger);
  spdlog::flush_on(spdlog::level::err);
}

#ifdef PLATFORM_ANDROID
void Log::setup_android_sink(std::vector<spdlog::sink_ptr>& sinks)
{
  auto android_sink = std::make_shared<spdlog::sinks::android_sink_mt>("AERO");
  android_sink->set_pattern("[%n] [%l] %v");

  sinks.push_back(android_sink);
}
#endif

void Log::set_level(spdlog::level::level_enum level)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  if (s_core_logger)
    s_core_logger->set_level(level);
  if (s_client_logger)
    s_client_logger->set_level(level);
#ifdef AERO_SERVER
  if (s_server_logger)
    s_server_logger->set_level(level);
#endif
}
std::vector<spdlog::sink_ptr> Log::create_sinks(const LogConfig& config)
{
  std::vector<spdlog::sink_ptr> sinks;

#ifdef PLATFORM_ANDROID
  setup_android_sink(sinks);

#elif defined(AERO_EDITOR)
  if (config.enable_console)
  {
    setup_console_sink(sinks);
  }
  if (config.enable_file_logging)
  {
    setup_file_sink(sinks, config.log_file_path);
  }

  if (config.enable_imgui_console)
  {
    setup_imgui_sink(sinks);
  }
#else
  if (config.enable_console)
  {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("%^[%T.%e] [%n] [%l] %v%$");
    sinks.push_back(console_sink);
  }
  if (config.enable_file_logging)
  {
    try
    {
      auto file_sink =
          std::make_shared<spdlog::sinks::basic_file_sink_mt>(config.log_file_path, true);
      file_sink->set_pattern("[%Y-%m-%d %T.%e] [%n] [%l] %v");
      sinks.push_back(file_sink);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
      std::cerr << "Log file creation failed: " << ex.what() << std::endl;
    }
  }
#endif
  return sinks;
}
void Log::enable_file_logging(const std::string& path)
{
  std::lock_guard<std::mutex> lock(s_mutex);
  if (!s_current_config.enable_file_logging)
  {
    s_current_config.enable_file_logging = true;
    if (!path.empty())
    {
      s_current_config.log_file_path = path;
    }
    // Recreate loggers with new config
    create_loggers(s_current_config);
  }
}

void Log::disable_file_logging()
{
  std::lock_guard<std::mutex> lock(s_mutex);
  if (s_current_config.enable_file_logging)
  {
    s_current_config.enable_file_logging = false;
    create_loggers(s_current_config);
  }
}

void Log::flush()
{
  if (s_core_logger)
    s_core_logger->flush();
  if (s_client_logger)
    s_client_logger->flush();
#ifdef AERO_SERVER
  if (s_server_logger)
    s_server_logger->flush();
#endif
}

}  // namespace ag
