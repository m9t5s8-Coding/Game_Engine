#include "Core/Log.hpp"

#include <Application/AppSettings.hpp>
#include <filesystem>

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#elif defined(PLATFORM_LINUX)
  #include <limits.h>
  #include <unistd.h>
#endif

namespace ag
{
AppSettings::Mode AppSettings::s_mode                 = AppSettings::Mode::ProjectManager;
std::string       AppSettings::s_settings_path        = "";
std::string       AppSettings::s_recent_projects_path = "";

void AppSettings::load()
{
  create_app_folder();
}

std::string AppSettings::get_appdata_path()
{
#ifdef PLATFORM_WINDOWS
  const char* appdata = std::getenv("APPDATA");
  if (appdata)
    return std::string(appdata);
  else
    return ".";
#elif defined(PLATFORM_LINUX)
  const char* home = std::getenv("HOME");
  AERO_CORE_INFO("Path:{0}", home);
  if (home)
    return std::string(home) + "/.config";
  else
    return ".";
#endif
}

void AppSettings::create_app_folder()
{
  std::string app_data_path = get_appdata_path();
  Helper::normalize_path(app_data_path);

  std::string app_folder = app_data_path + "/AEROEngine";
  s_settings_path        = app_folder + "/settings.json";
  s_recent_projects_path = app_folder + "/recent_projects.json";

  // create app folder
  try
  {
    if (!std::filesystem::exists(app_folder))
      std::filesystem::create_directories(app_folder);
  }
  catch (const std::filesystem::filesystem_error& e)
  {
    AERO_CORE_ERROR("Failed to create app folder: {0}", e.what());
    return;
  }

  // Create or Load Settings.json
  create_load_settings();

  // create recent projects file
}

void AppSettings::create_load_settings()
{
  Helper::makefile_read_only(s_settings_path, false);
  // create app setting json file
  if (!std::filesystem::exists(s_settings_path))
  {
    try
    {
      std::ofstream file(s_settings_path);
      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to Open File: {0}", s_settings_path);
        return;
      }
      json j;
      Helper::save_json(j,
                        "Mode",
                        static_cast<int>(Mode::ProjectManager),
                        (int)Mode::ProjectManager);
      file << j.dump(4);
      file.close();
      Helper::makefile_read_only(s_settings_path);
      AERO_CORE_INFO("Settings File created at: {0}", s_settings_path)
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception creating settings.json: {0}", e.what());
    };
  }
  else
  {
    try
    {
      std::ifstream file(s_settings_path);

      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to Open File: {0}", s_settings_path);
        return;
      }

      file.seekg(0, std::ios::end);
      if (file.tellg() == 0)
      {
        AERO_CORE_WARN("Settings Path is empty using Defaults");
        file.close();
        Helper::makefile_read_only(s_settings_path);
        return;
      }
      file.seekg(0, std::ios::beg);

      json j;
      j = json::parse(file, nullptr, false);
      file.close();
      Helper::makefile_read_only(s_settings_path);

      if (j.is_discarded())
      {
        AERO_CORE_ERROR("Settings file has invalid JSON, using defaults");
        return;
      }

      int mode;
      Helper::load_json(j, "Mode", mode, (int)Mode::ProjectManager);
      s_mode = static_cast<Mode>(mode);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception loading settings: {0}", e.what());
    }
  }
}

void AppSettings::create_load_recent_projects()
{
  Helper::makefile_read_only(s_recent_projects_path, false);
  if (!std::filesystem::exists(s_recent_projects_path))
  {
    try
    {
      std::ofstream file(s_recent_projects_path);
      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to create recent projects file");
        return;
      }
      file << "[]";
      file.close();
      Helper::makefile_read_only(s_recent_projects_path);
      AERO_CORE_INFO("Recent project file created at: {0}", s_recent_projects_path);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception creating recent projects: {0}", e.what());
    }
  }
}

void AppSettings::reload_app()
{
  std::string exePath = get_exe_path();

#ifdef PLATFORM_WINDOWS
  STARTUPINFOA        si = {sizeof(si)};
  PROCESS_INFORMATION pi;

  // Start new process
  if (CreateProcessA(exePath.c_str(),
                     nullptr,
                     nullptr,
                     nullptr,
                     FALSE,
                     0,
                     nullptr,
                     nullptr,
                     &si,
                     &pi))
  {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
#elif defined(PLATFORM_LINUX)
  // Fork and exec on Linux
  pid_t pid = fork();
  if (pid == 0)
  {
    // Child process
    execl(exePath.c_str(), exePath.c_str(), nullptr);
    // If execl returns, it failed
    AERO_CORE_ERROR("Failed to restart application");
    exit(1);
  }
  else if (pid < 0)
  {
    AERO_CORE_ERROR("Failed to fork process");
  }
#endif

  exit(0);
}

std::string AppSettings::get_exe_path()
{
#ifdef PLATFORM_WINDOWS
  char buffer[MAX_PATH];
  GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  return std::string(buffer);
#elif defined(PLATFORM_LINUX)
  char    buffer[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len != -1)
  {
    buffer[len] = '\0';
    return std::string(buffer);
  }
  else
  {
    AERO_CORE_ERROR("Failed to get executable path");
    return "";
  }
#endif
}
}  // namespace ag
