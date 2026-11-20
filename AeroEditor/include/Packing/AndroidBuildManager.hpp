#pragma once

#include <Packing/GameExporter.hpp>

#include <atomic>
#include <ios>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace ag {

class AndroidBuildManager {
public:
  enum class Status { Idle, Preparing, Exporting, Building, Done, Failed };
  enum class BuildType { Debug, Release };

  struct Config {
    std::string android_dir;
    BuildType   build_type = BuildType::Debug;
    bool        offline    = false;
    bool        clean      = false;
  };

  AndroidBuildManager();
  ~AndroidBuildManager();

  static std::string detect_android_dir();
  static bool        has_android_project(const std::string& dir);
  static std::string default_apk_path(const std::string& dir, BuildType type);

  void start(const Config& config);
  void update();

  bool is_busy() const;
  void clear_status();

  std::vector<std::string> get_log() const;
  Status                   get_status() const;
  float                    get_progress() const;
  std::string              get_status_message() const;
  std::string              get_error_message() const;
  std::string              get_apk_path() const;

private:
  GameExporter m_exporter;

  Config      m_config;
  Status      m_status = Status::Idle;
  std::string m_status_message;
  std::string m_error_message;
  std::string m_apk_path;
  float       m_progress = 0.0f;

  mutable std::mutex m_state_mutex;

  std::vector<std::string> m_log;
  mutable std::mutex       m_log_mutex;

  std::thread       m_build_thread;
  std::atomic<bool> m_process_running = false;
  std::atomic<bool> m_build_finished  = false;

  std::string m_temp_dir;
  std::string m_gradle_log;
  std::streampos m_gradle_log_pos  = 0;
  size_t          m_gradle_tasks   = 0;
  size_t          m_export_log_index = 0;

  void thread_func();
  int  run_gradle();

  void pump_export_log();
  void pump_gradle_log();
  void parse_gradle_line(const std::string& line);

  void log_msg(const std::string& msg);
  void set_status(Status status);
  void set_progress(float progress);
  void set_error(const std::string& message);
  void set_result_message(const std::string& message);
  void set_apk_path(const std::string& path);
};

}  // namespace ag
