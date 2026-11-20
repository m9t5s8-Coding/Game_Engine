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

// The project-managed folders that the editor may read/write through its pickers.
enum class ProjectDirectory
{
  Scenes,
  Scripts,
  Assets
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

  // Absolute, normalized filesystem path of a managed project directory.
  std::string get_directory_path(ProjectDirectory kind) const;
  // Active-project variants return an empty string/false when no project is loaded.
  static std::string get_active_directory_path(ProjectDirectory kind);

  // An existing directory that can be used as the initial folder of a file dialog.
  static std::string get_dialog_directory(ProjectDirectory kind);

  // True when path is the managed directory itself or one of its descendants.
  // Subfolders are allowed; ".." escapes are not.
  bool is_path_in_directory(const std::string& path, ProjectDirectory kind) const;
  static bool is_path_in_active_directory(const std::string& path, ProjectDirectory kind);

  // Converts an absolute path inside a managed directory to the project-relative
  // form used by scenes/scripts/assets (for example "/sub/name.lua").
  // Returns an empty string when the path is outside the managed directory.
  std::string to_project_relative(const std::string& path, ProjectDirectory kind) const;
  static std::string to_active_relative_path(const std::string& path, ProjectDirectory kind);

  // Converts a project-relative path (with or without a leading '/') back to an
  // absolute filesystem path. Returns an empty string for paths that escape root.
  std::string resolve_relative_path(const std::string& relative_path, ProjectDirectory kind) const;
  static std::string resolve_active_relative_path(const std::string& relative_path,
                                                  ProjectDirectory kind);

  static const char* get_directory_name(ProjectDirectory kind);

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
