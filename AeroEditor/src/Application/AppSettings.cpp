#include "Core/Log.hpp"

#include <Application/AppSettings.hpp>
#include <Core/Core.hpp>
#include <filesystem>
#include <Helper.hpp>

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
#elif defined(PLATFORM_LINUX) && !defined(PLATFORM_ANDROID)
  #include <limits.h>
  #include <unistd.h>
#endif

namespace ag
{
AppSettings::Mode AppSettings::s_mode                 = AppSettings::Mode::ProjectManager;
std::string       AppSettings::s_settings_path        = "";
std::string       AppSettings::s_recent_projects_path = "";
bool              AppSettings::s_loaded               = false;

void AppSettings::load()
{
  if (s_loaded)
    return;
  create_app_folder();
  s_loaded = true;
}

void AppSettings::ensure_loaded()
{
  if (s_loaded)
    return;

  if (!s_loaded)
  {
    create_app_folder();
    s_loaded = true;
  }
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
  if (home)
    return std::string(home) + "/.config";
  else
    return ".";

#elif defined(PLATFORM_ANDROID)
  extern android_app* g_android_app;

  if (g_android_app && g_android_app->activity)
  {
    std::string internal_path = g_android_app->activity->internalDataPath;
    AERO_CORE_INFO("Android Internal Path: {0}", internal_path.c_str());
    return internal_path;
  }
  else
  {
    return "/data/data/com.yourname.aeroengine/files";
  }
#endif
}

void AppSettings::create_app_folder()
{
  std::string app_data_path = get_appdata_path();
  AERO_CORE_INFO("App Data Path:{0}", app_data_path);
  if (app_data_path.empty())
  {
    AERO_CORE_ERROR("App Data Path is Empty");
    return;
  }
  Helper::normalize_path(app_data_path);

  std::string app_folder = app_data_path + "/AEROEngine";
  s_settings_path        = app_folder + "/settings.json";
  s_recent_projects_path = app_folder + "/recent_projects.json";

  AERO_CORE_INFO("App folder path: {}", app_folder);

  // create app folder
  try
  {
    if (!std::filesystem::exists(app_folder))
    {
      AERO_CORE_INFO("Creating app folder: {}", app_folder);
      if (!std::filesystem::create_directories(app_folder))
      {
        AERO_CORE_ERROR("Failed to create directories: {}", app_folder);
        return;
      }
      AERO_CORE_INFO("App folder created successfully");
    }
    else
    {
      AERO_CORE_INFO("App folder already exists");
    }
  }
  catch (const std::filesystem::filesystem_error& e)
  {
    AERO_CORE_ERROR("Failed to create app folder: {}", e.what());
    AERO_CORE_ERROR("Path: {}", app_folder);
    return;
  }

  // Create or Load Settings.json
  create_load_settings();
  create_load_recent_projects();
}

void AppSettings::create_load_settings()
{
  Helper::makefile_read_only(s_settings_path, false);

  AERO_CORE_INFO("Settings path: {}", s_settings_path);

  if (!std::filesystem::exists(s_settings_path))
  {
    try
    {
      AERO_CORE_INFO("Creating new settings file");
      std::ofstream file(s_settings_path);
      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to Open File: {}", s_settings_path);
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
      AERO_CORE_INFO("Settings File created at: {}", s_settings_path);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception creating settings.json: {}", e.what());
    }
  }
  else
  {
    try
    {
      AERO_CORE_INFO("Loading existing settings file");
      std::ifstream file(s_settings_path);

      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to Open File: {}", s_settings_path);
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
      AERO_CORE_INFO("Settings loaded, mode: {}", mode);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception loading settings: {}", e.what());
    }
  }
}

void AppSettings::create_load_recent_projects()
{
  Helper::makefile_read_only(s_recent_projects_path, false);

  AERO_CORE_INFO("Recent projects path: {}", s_recent_projects_path);

  if (!std::filesystem::exists(s_recent_projects_path))
  {
    try
    {
      AERO_CORE_INFO("Creating new recent projects file");
      std::ofstream file(s_recent_projects_path);
      if (!file.is_open())
      {
        AERO_CORE_ERROR("Failed to create recent projects file");
        return;
      }
      file << "[]";
      file.close();
      Helper::makefile_read_only(s_recent_projects_path);
      AERO_CORE_INFO("Recent project file created at: {}", s_recent_projects_path);
    }
    catch (const std::exception& e)
    {
      AERO_CORE_ERROR("Exception creating recent projects: {}", e.what());
    }
  }
  else
  {
    AERO_CORE_INFO("Recent projects file already exists");
  }
}

void AppSettings::reload_app()
{
  std::string exePath = get_exe_path();

  if (exePath.empty())
  {
    AERO_CORE_ERROR("Cannot reload app - executable path is empty");
    return;
  }

#ifdef PLATFORM_WINDOWS
  STARTUPINFOA        si = {sizeof(si)};
  PROCESS_INFORMATION pi;

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
#elif defined(PLATFORM_LINUX) && !defined(PLATFORM_ANDROID)
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

#elif defined(PLATFORM_ANDROID)
  // On Android, we don't have a direct executable path
  if (g_android_app && g_android_app->activity)
  {
    std::string path = g_android_app->activity->externalDataPath;
    if (path.empty())
    {
      path = "/data/app/com.yourname.aeroengine";
    }
    return path;
  }
  return "";

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
