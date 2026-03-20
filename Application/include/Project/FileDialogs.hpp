#pragma once

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
  static std::string open_file(const char* filter);

  static std::string save_file(const char* filter);

  static std::string select_folder(const char* title = "Select Folder");

  static void         run_exe(const std::wstring& exe_path);
  static void         run_exe(const std::wstring& exe_path, const std::wstring& arg);
  static std::wstring get_exe_folder();
};
}  // namespace ag
