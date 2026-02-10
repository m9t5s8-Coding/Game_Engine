#include <Application/AppSettings.hpp>

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
#elif defined(PLATFORM_LINUX)
    #include <unistd.h>
    #include <limits.h>
#endif

namespace ag
{
    AppSettings::Mode AppSettings::s_mode = AppSettings::Mode::ProjectManager;
    std::string AppSettings::s_settings_path = "";
    std::string AppSettings::s_recent_projects_path = "";

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
       s_settings_path = app_folder + "/settings.json";
       s_recent_projects_path = app_folder + "/recent_projects.json";

       // create app folder
       if (!std::filesystem::exists(app_folder))
          std::filesystem::create_directories(app_folder);

       // create app setting json file
       if (!std::filesystem::exists(s_settings_path))
       {
          Helper::makefile_read_only(s_settings_path, false);
          std::ofstream file(s_settings_path);
          if (!file.is_open())
          {
             AERO_CORE_ERROR("Failed to Open File: {0}", s_settings_path);
          }
          json j;
          Helper::save_json(j, "Mode", static_cast<int>(Mode::ProjectManager));
          file << j.dump(4);
          file.close();
          Helper::makefile_read_only(s_settings_path);
          AERO_CORE_INFO("Settings File created at: {0}", s_settings_path);
       }
       else
       {
          json j;
          Helper::makefile_read_only(s_settings_path, false);
          std::ifstream file(s_settings_path);

          if (!file.is_open())
          {
             AERO_CORE_ERROR("Failed to Open File: {0}", s_settings_path);
             return;
          }
          file >> j;
          file.close();
          Helper::makefile_read_only(s_settings_path);

          int mode = 0;
          Helper::load_json(j, "Mode", mode);
          s_mode = static_cast<Mode>(mode);
       }

       // create recent projects file
       if (!std::filesystem::exists(s_recent_projects_path))
       {
          Helper::makefile_read_only(s_recent_projects_path, false);
          std::ofstream file(s_recent_projects_path);
          if (!file.is_open())
          {
             AERO_CORE_ERROR("Failed to create recent projects file");
          }
          file.close();
          Helper::makefile_read_only(s_recent_projects_path);
       }
       else
       {
          // save recent projects
       }
    }

    void AppSettings::reload_app()
    {
       std::string exePath = get_exe_path();

#ifdef PLATFORM_WINDOWS
       STARTUPINFOA si = { sizeof(si) };
       PROCESS_INFORMATION pi;

       // Start new process
       if (CreateProcessA(
          exePath.c_str(),
          nullptr,
          nullptr, nullptr, FALSE,
          0, nullptr, nullptr,
          &si, &pi))
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
       char buffer[PATH_MAX];
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
}