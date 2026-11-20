#pragma once

#include <string>

namespace ag
{
enum class FileType
{
  Project_File,
  Scene_File,
  Folder
};
class FileDialogs
{
public:
  // initial_directory is used as the dialog's starting folder when provided.
  static std::string open_file(const char* filter,
                               const std::string& initial_directory = std::string());

  static std::string save_file(const char* filter,
                               const std::string& initial_directory = std::string());

  static std::string select_folder(const char* title = "Select Folder",
                                   const std::string& initial_directory = std::string());

  static void         run_exe(const std::wstring& exe_path);
  static void         run_exe(const std::wstring& exe_path, const std::wstring& arg);
  static std::wstring get_exe_folder();
};
}  // namespace ag
