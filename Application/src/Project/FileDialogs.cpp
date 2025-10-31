#include <Project/FileDialogs.hpp>
#include <commdlg.h>
#include <Windows.h>
#include <shobjidl.h> 
#include <string>


namespace ag
{
  std::string FileDialogs::open_file(const char* filter)
  {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn))
      return ofn.lpstrFile;

    return std::string();
  }

  std::string FileDialogs::save_file(const char* filter)
  {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn))
      return ofn.lpstrFile;

    return std::string();
  }

  std::string FileDialogs::select_folder(const char* title)
  {
    std::string result;

    IFileDialog* pFileDialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
      IID_PPV_ARGS(&pFileDialog));

    if (FAILED(hr))
      return "";

    DWORD options;
    pFileDialog->GetOptions(&options);
    pFileDialog->SetOptions(options | FOS_PICKFOLDERS);

    if (title)
    {
      std::wstring wtitle(title, title + strlen(title));
      pFileDialog->SetTitle(wtitle.c_str());
    }

    hr = pFileDialog->Show(nullptr);
    if (SUCCEEDED(hr))
    {
      IShellItem* pItem = nullptr;
      hr = pFileDialog->GetResult(&pItem);
      if (SUCCEEDED(hr))
      {
        PWSTR path;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &path);
        if (SUCCEEDED(hr))
        {
          std::wstring ws(path);
          result = std::string(ws.begin(), ws.end());
          CoTaskMemFree(path);
        }
        pItem->Release();
      }
    }

    pFileDialog->Release();
    return result;
  }
}
