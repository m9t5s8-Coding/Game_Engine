#ifdef PLATFORM_WINDOWS

#include <Project/FileDialogs.hpp>
#include <commdlg.h>
#include <Windows.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "ole32.lib")

namespace ag
{
  namespace
  {
    // UTF-8 to UTF-16
    std::wstring utf8_to_wide(const std::string& utf8)
    {
      if (utf8.empty()) return L"";

      int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(),
        (int)utf8.size(), nullptr, 0);
      if (size_needed == 0) return L"";

      std::wstring wide(size_needed, 0);
      MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(),
        &wide[0], size_needed);
      return wide;
    }

    // UTF-16 to UTF-8
    std::string wide_to_utf8(const std::wstring& wide)
    {
      if (wide.empty()) return "";

      int size_needed = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(),
        (int)wide.size(), nullptr, 0, nullptr, nullptr);
      if (size_needed == 0) return "";

      std::string utf8(size_needed, 0);
      WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), (int)wide.size(),
        &utf8[0], size_needed, nullptr, nullptr);
      return utf8;
    }
  }

  std::string FileDialogs::open_file(const char* filter)
  {
    OPENFILENAMEW ofn = { 0 };
    wchar_t szFile[260] = { 0 };

    std::wstring filterW;
    if (filter)
    {
      std::string filterStr = filter;
      size_t pos = 0;
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

    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = filterW.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameW(&ofn))
    {
      return wide_to_utf8(szFile);
    }

    return std::string();
  }

  std::string FileDialogs::save_file(const char* filter)
  {
    OPENFILENAMEW ofn = { 0 };
    wchar_t szFile[260] = { 0 };

    std::wstring filterW;
    if (filter)
    {
      std::string filterStr = filter;
      size_t pos = 0;
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

    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
    ofn.lpstrFilter = filterW.c_str();
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    // Add default extension if none provided
    std::wstring defaultExt = L"";
    ofn.lpstrDefExt = defaultExt.c_str();

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
    bool comInitialized = SUCCEEDED(hr);

    IFileDialog* pFileDialog = nullptr;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
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
        hr = pFileDialog->GetResult(&pItem);
        if (SUCCEEDED(hr))
        {
          wchar_t* path = nullptr;
          hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &path);
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
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    wchar_t* cmdLine = _wcsdup(exe_path.c_str());

    BOOL success = CreateProcessW(
      nullptr,
      cmdLine,
      nullptr,
      nullptr,
      FALSE,
      0,
      nullptr,
      nullptr,
      &si,
      &pi
    );

    free(cmdLine);

    if (success)
    {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    }
    else
    {
      DWORD error = GetLastError();
      wchar_t errorMsg[256];
      swprintf_s(errorMsg, L"Failed to launch app (Error: %lu)", error);
      MessageBoxW(nullptr, errorMsg, L"Error", MB_OK | MB_ICONERROR);
    }
  }

  std::wstring FileDialogs::get_exe_folder()
  {
    wchar_t path[MAX_PATH] = { 0 };
    DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);

    if (length == 0 || length >= MAX_PATH)
      return L"";

    PathRemoveFileSpecW(path);
    return std::wstring(path);
  }
}

#endif