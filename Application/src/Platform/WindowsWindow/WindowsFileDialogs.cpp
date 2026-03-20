#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)

  #include <Project/FileDialogs.hpp>

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
}  // namespace

std::string FileDialogs::open_file(const char* filter)
{
  OPENFILENAMEW ofn         = {0};
  wchar_t       szFile[260] = {0};

  std::wstring filterW;
  if (filter)
  {
    std::string filterStr = filter;
    size_t      pos       = 0;
    while ((pos = filterStr.find(';', pos)) != std::string::npos)
    {
      filterStr.replace(pos, 1, "\0", 1);
    }
    filterW = utf8_to_wide(filterStr);
  }
  else
  {
    filterW = L"All Files (*.*)\0*.*\0\0";
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

std::string FileDialogs::save_file(const char* filter)
{
  OPENFILENAMEW ofn         = {0};
  wchar_t       szFile[260] = {0};

  std::wstring filterW;
  if (filter)
  {
    std::string filterStr = filter;
    size_t      pos       = 0;
    while ((pos = filterStr.find(';', pos)) != std::string::npos)
    {
      filterStr.replace(pos, 1, "\0", 1);
    }
    filterW = utf8_to_wide(filterStr);
  }
  else
  {
    filterW = L"All Files (*.*)\0*.*\0\0";
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

std::string FileDialogs::select_folder(const char* title)
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

void FileDialogs::run_exe(const std::wstring& exe_path)
{
  STARTUPINFOW        si = {0};
  PROCESS_INFORMATION pi = {0};
  si.cb                  = sizeof(si);

  wchar_t* cmdLine = _wcsdup(exe_path.c_str());

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

void add_filters_to_dialog(GtkFileChooser* chooser, const char* filter)
{
  if (!filter || filter[0] == '\0')
  {
    GtkFileFilter* all_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(all_filter, "All Files");
    gtk_file_filter_add_pattern(all_filter, "*");
    gtk_file_chooser_add_filter(chooser, all_filter);
    return;
  }

  std::string filterStr = filter;
  size_t      pos       = 0;

  while (pos < filterStr.length())
  {
    size_t pipe1 = filterStr.find('|', pos);
    if (pipe1 == std::string::npos)
      break;

    std::string name = filterStr.substr(pos, pipe1 - pos);

    size_t      pipe2 = filterStr.find('|', pipe1 + 1);
    std::string pattern;
    if (pipe2 != std::string::npos)
    {
      pattern = filterStr.substr(pipe1 + 1, pipe2 - pipe1 - 1);
      pos     = pipe2 + 1;
    }
    else
    {
      pattern = filterStr.substr(pipe1 + 1);
      pos     = filterStr.length();
    }

    GtkFileFilter* file_filter = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter, name.c_str());
    gtk_file_filter_add_pattern(file_filter, pattern.c_str());
    gtk_file_chooser_add_filter(chooser, file_filter);
  }
}
}  // namespace

std::string FileDialogs::open_file(const char* filter)
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

std::string FileDialogs::save_file(const char* filter)
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

std::string FileDialogs::select_folder(const char* title)
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
