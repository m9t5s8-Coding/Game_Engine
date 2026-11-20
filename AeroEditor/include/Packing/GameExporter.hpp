#pragma once

#include <atomic>
#include <future>
#include <mutex>
#include <nlohmann/json.hpp>
#include <Project/GamePacker.hpp>
#include <string>
#include <vector>

namespace ag {

class GameExporter {
public:
  enum class Status {
    Idle,
    Preparing,
    Packing,
    Copying,
    Done,
    Failed
  };

private:
  struct Config {
    std::string output_folder;
    std::string pak_name;
    std::string exe_name;
    std::string project_dir;
    std::string project_name;
    nlohmann::json settings;
  };

  Config m_config;

  Status      m_status = Status::Idle;
  std::string m_status_message;
  std::string m_error_message;
  float       m_progress = 0.0f;

  mutable std::mutex m_state_mutex;

  std::vector<std::string> m_log;
  mutable std::mutex       m_log_mutex;

  std::future<void> m_export_future;
  std::atomic<bool> m_running = false;

  std::string m_temp_dir;

public:
  GameExporter();
  ~GameExporter();

  void refresh_project_info();
  std::vector<std::string> get_log() const;

  std::string get_project_name() const;
  std::string get_project_dir() const;
  std::string get_output_folder() const;
  std::string get_pak_name() const;
  std::string get_exe_name() const;

  void set_project_name(const std::string& name);
  void set_project_dir(const std::string& dir);
  void set_output_folder(const std::string& folder);
  void set_pak_name(const std::string& name);
  void set_exe_name(const std::string& name);

  void clear_status();

  std::string get_error_message() const;
  Status      get_status() const;
  float       get_progress() const;
  std::string get_status_message() const;

  bool is_busy();

  void start_export();
  void browse_output_folder();

private:
  Config snapshot_config() const;

  void export_thread_func(Config config);

  void log_msg(const std::string& msg);

  void set_status(Status status);
  void set_progress(float progress);
  void set_error(const std::string& message);
  void set_result_message(const std::string& message);
  void finish_if_complete();
};
}  // namespace ag
