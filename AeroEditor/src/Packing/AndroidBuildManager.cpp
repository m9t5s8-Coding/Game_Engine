#include <Packing/AndroidBuildManager.hpp>

#include <Core/Application.hpp>
#include <Project/Project.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#else
  #include <sys/wait.h>
#endif

namespace ag
{
namespace
{
namespace fs = std::filesystem;

#ifdef PLATFORM_WINDOWS
std::string shell_quote(const std::string& value)
{
  std::string result = "\"";
  for (char c : value)
  {
    if (c == '"')
      result += '\\';
    result += c;
  }
  result += '"';
  return result;
}
#else
std::string shell_quote(const std::string& value)
{
  std::string result = "'";
  for (char c : value)
  {
    if (c == '\'')
      result += "'\\''";
    else
      result += c;
  }
  result += '\'';
  return result;
}
#endif

int normalize_exit_code(int code)
{
#ifndef PLATFORM_WINDOWS
  if (WIFEXITED(code))
    return WEXITSTATUS(code);
  if (WIFSIGNALED(code))
    return 128 + WTERMSIG(code);
#endif
  return code;
}

std::string trim(const std::string& value)
{
  const auto first = value.find_first_not_of(" \t\r\n");
  if (first == std::string::npos)
    return std::string();
  const auto last = value.find_last_not_of(" \t\r\n");
  return value.substr(first, last - first + 1);
}
}  // namespace

AndroidBuildManager::AndroidBuildManager() = default;

AndroidBuildManager::~AndroidBuildManager()
{
  if (m_build_thread.joinable())
    m_build_thread.join();
}

std::string AndroidBuildManager::detect_android_dir()
{
  std::vector<fs::path> starts;
  starts.push_back(Application::get_exe_directory());
  starts.push_back(fs::current_path());

  for (const auto& start : starts)
  {
    fs::path current = fs::absolute(start);
    for (int depth = 0; depth < 10 && !current.empty(); ++depth)
    {
      if (has_android_project((current / "android").string()))
        return (current / "android").string();
      if (!current.has_parent_path() || current.parent_path() == current)
        break;
      current = current.parent_path();
    }
  }
  return std::string();
}

bool AndroidBuildManager::has_android_project(const std::string& dir)
{
  if (dir.empty())
    return false;

  std::error_code ec;
  if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
    return false;

  const bool has_wrapper = fs::exists(fs::path(dir) / "gradlew", ec) ||
                           fs::exists(fs::path(dir) / "gradlew.bat", ec);
  return has_wrapper && fs::exists(fs::path(dir) / "app" / "build.gradle", ec);
}

std::string AndroidBuildManager::default_apk_path(const std::string& dir, BuildType type)
{
  const char* variant = type == BuildType::Debug ? "debug" : "release";
  const char* name    = type == BuildType::Debug ? "app-debug.apk" : "app-release-unsigned.apk";
  return (fs::path(dir) / "app" / "build" / "outputs" / "apk" / variant / name).string();
}

void AndroidBuildManager::start(const Config& config)
{
  if (is_busy())
    return;
  if (m_build_thread.joinable())
    m_build_thread.join();

  m_config = config;

  {
    std::lock_guard<std::mutex> lock(m_log_mutex);
    m_log.clear();
  }
  {
    std::lock_guard<std::mutex> lock(m_state_mutex);
    m_status_message.clear();
    m_error_message.clear();
    m_apk_path.clear();
    m_progress        = 0.0f;
    m_export_log_index = 0;
    m_gradle_log_pos  = 0;
    m_gradle_tasks    = 0;
  }

  if (!has_android_project(m_config.android_dir))
  {
    set_error("Android project folder is missing gradlew or app/build.gradle.");
    set_status(Status::Failed);
    log_msg("ERROR: Invalid Android project folder: " + m_config.android_dir);
    return;
  }

  auto project = Project::get_active_project();
  if (!project || project->get_directory().empty())
  {
    set_error("No project loaded!");
    set_status(Status::Failed);
    log_msg("ERROR: No project loaded. Open or create a project first.");
    return;
  }

  const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
  m_temp_dir       = (fs::temp_directory_path() /
                ("aero_android_build_" + std::to_string(stamp)))
                     .string();
  std::error_code ec;
  fs::create_directories(m_temp_dir, ec);
  if (ec)
  {
    const std::string message = "Could not create a temporary build folder: " + ec.message();
    set_error(message);
    set_status(Status::Failed);
    log_msg("ERROR: " + message);
    return;
  }

  log_msg("=== ANDROID BUILD ===");
  log_msg("Project: " + project->get_name());
  log_msg("Android folder: " + m_config.android_dir);
  log_msg("Build type: " +
          std::string(m_config.build_type == BuildType::Debug ? "Debug" : "Release"));
  log_msg("Packaging the active project into game.pak...");

  m_exporter.clear_status();
  m_exporter.set_project_name(project->get_name());
  m_exporter.set_project_dir(project->get_directory());
  m_exporter.set_output_folder(m_temp_dir);
  m_exporter.set_pak_name("game.pak");
  m_exporter.set_exe_name("Sandbox");

  set_status(Status::Exporting);
  set_progress(0.05f);
  m_exporter.start_export();

  if (m_exporter.get_status() == GameExporter::Status::Failed)
  {
    const std::string message = m_exporter.get_error_message();
    set_error(message);
    set_status(Status::Failed);
    log_msg("ERROR: " + message);
  }

  pump_export_log();
}

void AndroidBuildManager::update()
{
  if (m_status == Status::Exporting)
  {
    pump_export_log();

    if (m_exporter.is_busy())
      return;

    pump_export_log();
    if (m_exporter.get_status() != GameExporter::Status::Done)
    {
      const std::string error = m_exporter.get_error_message();
      set_error(error.empty() ? "Project export failed." : error);
      set_status(Status::Failed);
      log_msg("ERROR: " + m_error_message);
      return;
    }

    log_msg("Project package ready: " + (fs::path(m_temp_dir) / "game.pak").string());
    log_msg("Starting Gradle build...");
    set_status(Status::Building);
    set_progress(0.30f);
    set_result_message("Building Android APK...");

    m_process_running = true;
    m_build_finished  = false;
    m_build_thread    = std::thread([this]() { thread_func(); });
    return;
  }

  if (m_status == Status::Building && m_build_finished && m_build_thread.joinable())
    m_build_thread.join();
}

void AndroidBuildManager::thread_func()
{
  const int exit_code = run_gradle();
  pump_gradle_log();

  if (exit_code == 0)
  {
    const std::string apk = default_apk_path(m_config.android_dir, m_config.build_type);
    set_apk_path(apk);
    set_progress(1.0f);
    set_result_message("APK built successfully.");
    set_status(Status::Done);
    log_msg("");
    log_msg("=== ANDROID BUILD COMPLETE ===");
    log_msg("APK: " + apk);

    std::error_code ec;
    fs::remove_all(m_temp_dir, ec);
    log_msg("Removed temporary build files.");
  }
  else
  {
    const std::string message =
        "Gradle build failed with exit code " + std::to_string(exit_code) + ".";
    set_error(message);
    set_result_message("Check the Android build log for details.");
    set_status(Status::Failed);
    log_msg("");
    log_msg("ERROR: " + message);
    log_msg("Temporary build files kept at: " + m_temp_dir);
  }

  m_build_finished = true;
}

int AndroidBuildManager::run_gradle()
{
  const fs::path android_dir(m_config.android_dir);
#ifdef PLATFORM_WINDOWS
  const fs::path gradlew = android_dir / "gradlew.bat";
#else
  const fs::path gradlew = android_dir / "gradlew";
#endif

  std::vector<std::string> args;
  if (m_config.clean)
    args.push_back(":app:clean");
  args.push_back(m_config.build_type == BuildType::Debug ? ":app:assembleDebug"
                                                          : ":app:assembleRelease");
  args.push_back("--console=plain");
  if (m_config.offline)
    args.push_back("--offline");
  args.push_back("-PaeroGamePak=" + (fs::path(m_temp_dir) / "game.pak").string());

  m_gradle_log     = (fs::path(m_temp_dir) / "gradle.log").string();
  m_gradle_log_pos = 0;
  m_gradle_tasks   = 0;
  {
    std::ofstream reset(m_gradle_log, std::ios::trunc);
  }

  std::string command;
#ifdef PLATFORM_WINDOWS
  const std::string batch_path = (fs::path(m_temp_dir) / "build_android.bat").string();
  {
    std::ofstream batch(batch_path);
    if (!batch)
    {
      log_msg("ERROR: Could not create Gradle batch file.");
      m_process_running = false;
      return -1;
    }

    batch << "@echo off\r\n";
    batch << "cd /d " << shell_quote(android_dir.string()) << "\r\n";
    batch << "call " << shell_quote(gradlew.string());
    for (const auto& arg : args)
      batch << ' ' << shell_quote(arg);
    batch << " > " << shell_quote(m_gradle_log) << " 2>&1\r\n";
    batch << "exit /b %ERRORLEVEL%\r\n";
  }
  command = "cmd /c " + shell_quote(batch_path);
#else
  std::string wrapper = "sh " + shell_quote(gradlew.string());
  for (const auto& arg : args)
    wrapper += ' ' + shell_quote(arg);

  command = "cd " + shell_quote(android_dir.string()) + " && " + wrapper + " > " +
            shell_quote(m_gradle_log) + " 2>&1";
#endif

  log_msg("Command: " + command);
  const int raw_code = std::system(command.c_str());
  m_process_running  = false;

  if (raw_code < 0)
  {
    log_msg("ERROR: Could not start the Gradle wrapper.");
    return -1;
  }
  return normalize_exit_code(raw_code);
}

void AndroidBuildManager::pump_export_log()
{
  const auto lines = m_exporter.get_log();
  if (m_export_log_index > lines.size())
    m_export_log_index = lines.size();

  for (; m_export_log_index < lines.size(); ++m_export_log_index)
    log_msg(lines[m_export_log_index]);
}

void AndroidBuildManager::pump_gradle_log()
{
  std::ifstream input(m_gradle_log);
  if (!input)
    return;

  input.seekg(m_gradle_log_pos);
  std::string line;
  while (std::getline(input, line))
  {
    log_msg(line);
    parse_gradle_line(line);
  }

  const std::streampos pos = input.tellg();
  if (pos != std::streampos(-1))
    m_gradle_log_pos = pos;
}

void AndroidBuildManager::parse_gradle_line(const std::string& line)
{
  const std::string text = trim(line);
  if (text.find("> Task :app:") != std::string::npos)
  {
    ++m_gradle_tasks;
    const float estimated = 0.30f + std::min(0.55f, static_cast<float>(m_gradle_tasks) * 0.015f);
    if (get_progress() < estimated)
      set_progress(estimated);
  }

  const auto percent_pos = text.find('%');
  if (percent_pos != std::string::npos)
  {
    size_t number_start = percent_pos;
    while (number_start > 0 && std::isdigit(static_cast<unsigned char>(text[number_start - 1])))
      --number_start;
    if (number_start < percent_pos)
    {
      try
      {
        const int percent = std::stoi(text.substr(number_start, percent_pos - number_start));
        if (percent >= 0 && percent <= 100)
        {
          const float estimated = 0.30f + 0.55f * (static_cast<float>(percent) / 100.0f);
          if (get_progress() < estimated)
            set_progress(estimated);
        }
      }
      catch (const std::exception&)
      {
      }
    }
  }

  if (text.find("BUILD SUCCESSFUL") != std::string::npos)
    set_progress(0.95f);
  else if (text.find("BUILD FAILED") != std::string::npos)
    set_progress(0.95f);
}

void AndroidBuildManager::log_msg(const std::string& msg)
{
  std::lock_guard<std::mutex> lock(m_log_mutex);
  m_log.push_back(msg);
  if (m_log.size() > 2000)
    m_log.erase(m_log.begin(), m_log.begin() + (m_log.size() - 2000));
}

void AndroidBuildManager::set_status(Status status)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status = status;
}

void AndroidBuildManager::set_progress(float progress)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_progress = std::clamp(progress, 0.0f, 1.0f);
}

void AndroidBuildManager::set_error(const std::string& message)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_error_message = message;
}

void AndroidBuildManager::set_result_message(const std::string& message)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status_message = message;
}

void AndroidBuildManager::set_apk_path(const std::string& path)
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_apk_path = path;
}

bool AndroidBuildManager::is_busy() const
{
  Status status = get_status();
  return status == Status::Preparing || status == Status::Exporting || status == Status::Building;
}

void AndroidBuildManager::clear_status()
{
  if (is_busy())
    return;

  std::lock_guard<std::mutex> lock(m_state_mutex);
  m_status         = Status::Idle;
  m_status_message.clear();
  m_error_message.clear();
  m_apk_path.clear();
  m_progress = 0.0f;
}

std::vector<std::string> AndroidBuildManager::get_log() const
{
  std::lock_guard<std::mutex> lock(m_log_mutex);
  return m_log;
}

AndroidBuildManager::Status AndroidBuildManager::get_status() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_status;
}

float AndroidBuildManager::get_progress() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_progress;
}

std::string AndroidBuildManager::get_status_message() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_status_message;
}

std::string AndroidBuildManager::get_error_message() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_error_message;
}

std::string AndroidBuildManager::get_apk_path() const
{
  std::lock_guard<std::mutex> lock(m_state_mutex);
  return m_apk_path;
}

}  // namespace ag
