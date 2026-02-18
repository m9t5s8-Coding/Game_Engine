#pragma once

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <Project/GamePacker.hpp>

namespace ag
{

  class GameExporter
  {
  public:
    enum class Status
    {
      Idle,
      Preparing,
      Packing,
      Copying,
      Done,
      Failed
    };

  private:
    std::string m_output_folder;
    std::string m_pak_name = "game.pak";
    std::string m_exe_name = "Sandbox.exe";

    std::string m_project_dir;
    std::string m_project_name;

    Status       m_status = Status::Idle;
    std::string  m_status_message;
    std::string  m_error_message;
    float        m_progress = 0.0f;

    std::vector<std::string> m_log;
    std::mutex               m_log_mutex;

    std::thread       m_export_thread;
    std::atomic<bool> m_running = false;

    std::string m_temp_dir;

  public:
    GameExporter();

    ~GameExporter();

    void refresh_project_info();
    const std::vector<std::string>& get_log() const { return m_log; }

    const std::string& get_project_name() const { return m_project_name; }
    const std::string& get_project_dir() const { return m_project_dir; }
    const std::string& get_output_folder() const { return m_output_folder; }
    const std::string& get_pak_name() const { return m_pak_name; }
    const std::string& get_exe_name() const { return m_exe_name; }

    void set_project_name(const std::string& name) { m_project_name = name; }
    void set_project_dir(const std::string& dir) { m_project_dir = dir; }
    void set_output_folder(const std::string& folder) { m_output_folder = folder; }
    void set_pak_name(const std::string& name) { m_pak_name = name; }
    void set_exe_name(const std::string& name) { m_exe_name = name; }

    void clear_status() {
      m_status = Status::Idle;
      m_progress = 0.0f;
      m_error_message.clear();
      m_status_message.clear();
    }


    const std::string& get_error_message() const { return m_error_message; }
    Status      get_status() const { return m_status; }
    float       get_progress() const { return m_progress; }
    std::string get_status_message() const { return m_status_message; }



    bool is_busy() const { return m_status != Status::Idle && m_status != Status::Done && m_status != Status::Failed; }


    void start_export();

    void browse_output_folder();

  private:
    void export_thread_func();

    void log_msg(const std::string& msg);
  };
}
