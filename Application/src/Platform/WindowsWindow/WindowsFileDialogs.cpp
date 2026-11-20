#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

  #include <Project/FileDialogs.hpp>

  #include <algorithm>

  #ifdef PLATFORM_WINDOWS
    #include <commdlg.h>
    #include <shlwapi.h>
    #include <shobjidl.h>
    #include <Windows.h>

    #include <codecvt>
    #include <locale>
    #include <string>
    #include <vector>

    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "shlwapi.lib")
    #pragma comment(lib, "ole32.lib")

namespace ag
{
namespace
{
std::wstring utf8_to_wide(const std::string& utf8)
{
  if (utf8.empty())
    return L"";

  int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), nullptr, 0);
  if (size_needed == 0)
    return L"";

  std::wstring wide(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &wide[0], size_needed);
  return wide;
}

std::string wide_to_utf8(const std::wstring& wide)
{
  if (wide.empty())
    return "";

  int size_needed =
      WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
  if (size_needed == 0)
    return "";

  std::string utf8(size_needed, 0);
  WideCharToMultiByte(CP_UTF8,
                      0,
                      wide.c_str(),
                      (int)wide.size(),
                      &utf8[0],
                      size_needed,
                      nullptr,
                      nullptr);
  return utf8;
}

// Filters are NUL-separated name/pattern pairs terminated by a second NUL.
size_t filter_data_length(const char* filter)
{
  if (!filter)
    return 0;
  size_t length = 0;
  while (length < 4096)
  {
    if (filter[length] == '\0' && filter[length + 1] == '\0')
      return length + 1;
    ++length;
  }
  return length;
}
}  // namespace

std::string FileDialogs::open_file(const char* filter, const std::string& initial_directory)
{
  OPENFILENAMEW ofn         = {0};
  wchar_t       szFile[260] = {0};

  std::wstring filterW;
  if (filter)
  {
    // Filters use the Win32 double-null terminated "name\0pattern\0" form.
    // Semicolons inside a pattern are valid and must be kept intact.
    filterW = utf8_to_wide(std::string(filter, filter_data_length(filter)));
  }
  else
  {
    filterW = L"All Files (*.*)\0*.*\0\0";
  }

  std::wstring initial_dir_w;
  if (!initial_directory.empty())
  {
    std::string normalized = initial_directory;
    std::replace(normalized.begin(), normalized.end(), '/', '\\');
    initial_dir_w       = utf8_to_wide(normalized);
    ofn.lpstrInitialDir = initial_dir_w.c_str();
  }

  ofn.lStructSize  = sizeof(OPENFILENAMEW);
  ofn.hwndOwner    = GetActiveWindow();
  ofn.lpstrFile    = szFile;
  ofn.nMaxFile     = sizeof(szFile) / sizeof(szFile[0]);
  ofn.lpstrFilter  = filterW.c_str();
  ofn.nFilterIndex = 1;
  ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

  if (GetOpenFileNameW(&ofn))
  {
    return wide_to_utf8(szFile);
  }

  return std::string();
}

std::string FileDialogs::save_file(const char* filter, const std::string& initial_directory)
{
  OPENFILENAMEW ofn         = {0};
  wchar_t       szFile[260] = {0};

  std::wstring filterW;
  if (filter)
  {
    filterW = utf8_to_wide(std::string(filter, filter_data_length(filter)));
  }
  else
  {
    filterW = L"All Files (*.*)\0*.*\0\0";
  }

  std::wstring initial_dir_w;
  if (!initial_directory.empty())
  {
    std::string normalized = initial_directory;
    std::replace(normalized.begin(), normalized.end(), '/', '\\');
    initial_dir_w       = utf8_to_wide(normalized);
    ofn.lpstrInitialDir = initial_dir_w.c_str();
  }

  ofn.lStructSize  = sizeof(OPENFILENAMEW);
  ofn.hwndOwner    = GetActiveWindow();
  ofn.lpstrFile    = szFile;
  ofn.nMaxFile     = sizeof(szFile) / sizeof(szFile[0]);
  ofn.lpstrFilter  = filterW.c_str();
  ofn.nFilterIndex = 1;
  ofn.Flags        = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

  // Add default extension if none provided
  std::wstring defaultExt = L"";
  ofn.lpstrDefExt         = defaultExt.c_str();

  if (GetSaveFileNameW(&ofn))
  {
    return wide_to_utf8(szFile);
  }

  return std::string();
}

std::string FileDialogs::select_folder(const char* title, const std::string& initial_directory)
{
  std::string result;

  HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
  bool    comInitialized = SUCCEEDED(hr);

  IFileDialog* pFileDialog = nullptr;
  hr                       = CoCreateInstance(CLSID_FileOpenDialog,
                        nullptr,
                        CLSCTX_INPROC_SERVER,
                        IID_PPV_ARGS(&pFileDialog));

  if (SUCCEEDED(hr))
  {
    DWORD options;
    pFileDialog->GetOptions(&options);
    pFileDialog->SetOptions(options | FOS_PICKFOLDERS);

    // Set title if provided
    if (title && title[0] != '\0')
    {
      std::wstring wtitle = utf8_to_wide(title);
      pFileDialog->SetTitle(wtitle.c_str());
    }

    if (!initial_directory.empty())
    {
      std::string normalized = initial_directory;
      std::replace(normalized.begin(), normalized.end(), '/', '\\');
      IShellItem* initial_folder = nullptr;
      if (SUCCEEDED(SHCreateItemFromParsingName(utf8_to_wide(normalized).c_str(),
                                                nullptr,
                                                IID_PPV_ARGS(&initial_folder))))
      {
        pFileDialog->SetFolder(initial_folder);
        initial_folder->Release();
      }
    }

    hr = pFileDialog->Show(nullptr);
    if (SUCCEEDED(hr))
    {
      IShellItem* pItem = nullptr;
      hr                = pFileDialog->GetResult(&pItem);
      if (SUCCEEDED(hr))
      {
        wchar_t* path = nullptr;
        hr            = pItem->GetDisplayName(SIGDN_FILESYSPATH, &path);
        if (SUCCEEDED(hr))
        {
          result = wide_to_utf8(path);
          CoTaskMemFree(path);
        }
        pItem->Release();
      }
    }
    pFileDialog->Release();
  }

  if (comInitialized)
  {
    CoUninitialize();
  }

  return result;
}

static std::wstring quote_windows_argument(const std::wstring& value)
{
  // Paths may contain spaces, so always quote CreateProcessW command-line tokens.
  return L"\"" + value + L"\"";
}

void FileDialogs::run_exe(const std::wstring& exe_path, const std::wstring& arg)
{
  STARTUPINFOW        si = {0};
  PROCESS_INFORMATION pi = {0};
  si.cb                  = sizeof(si);

  std::wstring command_line = quote_windows_argument(exe_path);
  if (!arg.empty())
    command_line += L" " + quote_windows_argument(arg);

  wchar_t* cmdLine = _wcsdup(command_line.c_str());

  BOOL success =
      CreateProcessW(nullptr, cmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

  free(cmdLine);

  if (success)
  {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
  else
  {
    DWORD   error = GetLastError();
    wchar_t errorMsg[256];
    swprintf_s(errorMsg, L"Failed to launch app (Error: %lu)", error);
    MessageBoxW(nullptr, errorMsg, L"Error", MB_OK | MB_ICONERROR);
  }
}

void FileDialogs::run_exe(const std::wstring& exe_path)
{
  run_exe(exe_path, std::wstring());
}

std::wstring FileDialogs::get_exe_folder()
{
  wchar_t path[MAX_PATH] = {0};
  DWORD   length         = GetModuleFileNameW(nullptr, path, MAX_PATH);

  if (length == 0 || length >= MAX_PATH)
    return L"";

  PathRemoveFileSpecW(path);
  return std::wstring(path);
}
}  // namespace ag

  #elif defined(PLATFORM_LINUX)
    #include <gtk/gtk.h>
    #include <libgen.h>
    #include <limits.h>
    #include <sys/wait.h>
    #include <unistd.h>

    #include <cstdlib>
    #include <cstring>
    #include <string>
    #include <utility>
    #include <vector>

namespace ag
{
namespace
{
void ensure_gtk_init()
{
  static bool initialized = false;
  if (!initialized)
  {
    gtk_init_check(nullptr, nullptr);
    initialized = true;
  }
}

size_t filter_data_length(const char* filter)
{
  if (!filter)
    return 0;
  size_t length = 0;
  while (length < 4096)
  {
    if (filter[length] == '\0' && filter[length + 1] == '\0')
      return length + 1;
    ++length;
  }
  return length;
}

std::string trim_filter_value(std::string value)
{
  size_t first = value.find_first_not_of(" \t");
  if (first == std::string::npos)
    return {};
  size_t last = value.find_last_not_of(" \t");
  return value.substr(first, last - first + 1);
}

void add_filters_to_dialog(GtkFileChooser* chooser, const char* filter)
{
  std::vector<std::pair<std::string, std::string>> filters;

  if (!filter || filter[0] == '\0')
  {
    filters.emplace_back("All Files", "*");
  }
  else
  {
    const size_t length = filter_data_length(filter);
    std::vector<std::string> tokens;
    for (size_t start = 0, i = 0; i < length; ++i)
    {
      if (filter[i] == '\0')
      {
        if (i > start)
          tokens.emplace_back(filter + start, i - start);
        start = i + 1;
      }
    }

    for (size_t i = 0; i + 1 < tokens.size(); i += 2)
      filters.emplace_back(tokens[i], tokens[i + 1]);

    // Accept the legacy "name|pattern|name|pattern" form as well.
    if (filters.empty())
    {
      const std::string legacy(filter);
      size_t            pos = 0;
      while (pos < legacy.size())
      {
        size_t name_end = legacy.find('|', pos);
        if (name_end == std::string::npos)
          break;
        size_t pattern_end = legacy.find('|', name_end + 1);
        std::string name   = legacy.substr(pos, name_end - pos);
        std::string pattern =
            legacy.substr(name_end + 1,
                          pattern_end == std::string::npos ? std::string::npos
                                                           : pattern_end - name_end - 1);
        filters.emplace_back(trim_filter_value(name), trim_filter_value(pattern));
        if (pattern_end == std::string::npos)
          break;
        pos = pattern_end + 1;
      }
    }
  }

  if (filters.empty())
    filters.emplace_back("All Files", "*");

  for (const auto& entry : filters)
  {
    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, entry.first.c_str());

    // A single Win32-style pattern may contain several semicolon-separated globs.
    size_t start = 0;
    while (start <= entry.second.size())
    {
      size_t      end     = entry.second.find(';', start);
      std::string pattern = trim_filter_value(entry.second.substr(
          start, end == std::string::npos ? std::string::npos : end - start));
      if (!pattern.empty())
        gtk_file_filter_add_pattern(file_filter, pattern.c_str());
      if (end == std::string::npos)
        break;
      start = end + 1;
    }

    gtk_file_chooser_add_filter(chooser, file_filter);
  }
}
}  // namespace

std::string FileDialogs::open_file(const char* filter, const std::string& initial_directory)
{
  ensure_gtk_init();

  GtkWidget* dialog = gtk_file_chooser_dialog_new("Open File",
                                                  nullptr,
                                                  GTK_FILE_CHOOSER_ACTION_OPEN,
                                                  "_Cancel",
                                                  GTK_RESPONSE_CANCEL,
                                                  "_Open",
                                                  GTK_RESPONSE_ACCEPT,
                                                  nullptr);

  add_filters_to_dialog(GTK_FILE_CHOOSER(dialog), filter);

  if (!initial_directory.empty())
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_directory.c_str());

  std::string result;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    if (filename)
    {
      result = filename;
      g_free(filename);
    }
  }

  gtk_widget_destroy(dialog);

  while (gtk_events_pending())
    gtk_main_iteration();

  return result;
}

std::string FileDialogs::save_file(const char* filter, const std::string& initial_directory)
{
  ensure_gtk_init();

  GtkWidget* dialog = gtk_file_chooser_dialog_new("Save File",
                                                  nullptr,
                                                  GTK_FILE_CHOOSER_ACTION_SAVE,
                                                  "_Cancel",
                                                  GTK_RESPONSE_CANCEL,
                                                  "_Save",
                                                  GTK_RESPONSE_ACCEPT,
                                                  nullptr);

  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
  add_filters_to_dialog(GTK_FILE_CHOOSER(dialog), filter);

  if (!initial_directory.empty())
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_directory.c_str());

  std::string result;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    if (filename)
    {
      result = filename;
      g_free(filename);
    }
  }

  gtk_widget_destroy(dialog);

  while (gtk_events_pending())
    gtk_main_iteration();

  return result;
}

std::string FileDialogs::select_folder(const char* title, const std::string& initial_directory)
{
  ensure_gtk_init();

  GtkWidget* dialog = gtk_file_chooser_dialog_new(title ? title : "Select Folder",
                                                  nullptr,
                                                  GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                  "_Cancel",
                                                  GTK_RESPONSE_CANCEL,
                                                  "_Select",
                                                  GTK_RESPONSE_ACCEPT,
                                                  nullptr);

  if (!initial_directory.empty())
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), initial_directory.c_str());

  std::string result;
  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    char* folder = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    if (folder)
    {
      result = folder;
      g_free(folder);
    }
  }

  gtk_widget_destroy(dialog);

  // Process pending events to clean up
  while (gtk_events_pending())
    gtk_main_iteration();

  return result;
}
void FileDialogs::run_exe(const std::wstring& exe_path, const std::wstring& arg)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string                                      exe_str = converter.to_bytes(exe_path);
  std::string                                      arg_str = converter.to_bytes(arg);

  pid_t pid = fork();
  if (pid == 0)
  {
    execl(exe_str.c_str(), exe_str.c_str(), arg_str.c_str(), nullptr);
    exit(1);  // execl only returns on failure
  }
  else if (pid < 0)
  {
    AERO_CORE_ERROR("Failed to fork process");
  }
}
void FileDialogs::run_exe(const std::wstring& exe_path)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string                                      exe_path_str = converter.to_bytes(exe_path);

  pid_t pid = fork();

  if (pid == 0)
  {
    execl(exe_path_str.c_str(), exe_path_str.c_str(), nullptr);
    exit(1);
  }
  else if (pid < 0)
  {
  }
}

std::wstring FileDialogs::get_exe_folder()
{
  char    path[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);

  if (count == -1)
    return L"";

  path[count] = '\0';

  char* dir = dirname(path);

  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(dir);
}
}  // namespace ag

  #endif

#endif
