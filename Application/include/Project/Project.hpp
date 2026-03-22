#pragma once

#include <Core/Core.hpp>
#include <Project/GlobalScripts.hpp>
#include <string>

namespace ag
{
struct ServerConfig
{
  bool enabled = false;

  uint16_t port        = 7777;
  int      max_clients = 5;
  int      tick_rate   = 20;

  std::vector<std::string> scripts;

  int  connection_timeout     = 10;
  bool auto_reconnect         = true;
  int  max_reconnection_tries = 3;

  std::string server_IP = "127.0.0.1";
};

struct Global_Scripts
{
  std::vector<std::string> global_scripts;
};

class Project
{
public:
  static AG_ref<Project> new_project(const std::string& path);
  static AG_ref<Project> load_project(const std::string& path);
  static AG_ref<Project> save_project();
  static void            save_active();

  static AG_ref<Project> get_active_project()
  {
    return s_active_project;
  }
  static void set_active_project(const AG_ref<Project> project)
  {
    s_active_project = project;
  }

  void init();
  void destroy();

  ServerConfig& get_server_config()
  {
    return m_server_config;
  }
  const ServerConfig& get_server_config() const
  {
    return m_server_config;
  }
  void set_server_config(const ServerConfig& config)
  {
    m_server_config = config;
  }

  Global_Scripts& get_global_scripts()
  {
    return m_global_scripts;
  }

  const Global_Scripts& get_global_scripts() const
  {
    return m_global_scripts;
  }

  void set_global_scripts(const Global_Scripts& scripts)
  {
    m_global_scripts = scripts;
  }

  GlobalScriptsManager& get_global_scripts_manager()
  {
    return *m_global_script_manager;
  }

  void set_name(const std::string& name)
  {
    m_name = name;
  }
  void set_directory(const std::string& directory)
  {
    m_directory = directory;
  }
  void set_project_file_directory(const std::string& directory)
  {
    m_project_file_path = m_directory;
  }
  void set_scene_directory(const std::string& scene_path)
  {
    m_scenes_directory = scene_path;
  }
  void set_assets_directory(const std::string& assets_path)
  {
    m_assets_directory = assets_path;
  }
  void set_scripts_directory(const std::string& scripts_path)
  {
    m_scripts_directory = scripts_path;
  }

  const std::string& get_name() const
  {
    return m_name;
  }
  const std::string& get_directory() const
  {
    return m_directory;
  }
  const std::string& get_project_file_directory() const
  {
    return m_project_file_path;
  }
  const std::string& get_scene_directory() const
  {
    return m_scenes_directory;
  }
  const std::string& get_assets_directory() const
  {
    return m_assets_directory;
  }
  const std::string& get_scripts_directory() const
  {
    return m_scripts_directory;
  }

  void save();

  bool m_project_loaded;

private:
  std::string m_name;
  std::string m_directory;
  std::string m_project_file_path;
  std::string m_assets_directory;
  std::string m_scenes_directory;
  std::string m_scripts_directory;
  bool        m_from_pak = false;

  ServerConfig   m_server_config;
  Global_Scripts m_global_scripts;

  AG_scope<GlobalScriptsManager> m_global_script_manager;

  inline static AG_ref<Project> s_active_project;
};
}  // namespace ag
