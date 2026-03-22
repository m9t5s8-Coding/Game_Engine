
#ifdef PLATFORM_ANDROID

  #include <Project/FileDialogs.hpp>

namespace ag
{

std::string FileDialogs::open_file(const char* filter)
{
  // Android uses system file picker via Java/JNI — not supported from C++ directly
  AERO_CORE_WARN("FileDialogs::open_file not supported on Android");
  return "";
}

std::string FileDialogs::save_file(const char* filter)
{
  AERO_CORE_WARN("FileDialogs::save_file not supported on Android");
  return "";
}

std::string FileDialogs::select_folder(const char* title)
{
  AERO_CORE_WARN("FileDialogs::select_folder not supported on Android");
  return "";
}

void FileDialogs::run_exe(const std::wstring& exe_path)
{
  // Android doesn't launch external executables
  AERO_CORE_WARN("FileDialogs::run_exe not supported on Android");
}

void FileDialogs::run_exe(const std::wstring& exe_path, const std::wstring& arg)
{
  AERO_CORE_WARN("FileDialogs::run_exe not supported on Android");
}

std::wstring FileDialogs::get_exe_folder()
{
  AERO_CORE_WARN("FileDialogs::get_exe_folder not supported on Android");
  return L"";
}
}  // namespace ag
#endif
