//
// Created by aogun on 2021/1/20.
//

#include "dialog_window.h"
#include "windows.h"
#include <sstream>
#include <shobjidl.h>
#include "mm_log.h"

extern HWND g_hwnd;

dialog_window::dialog_window() {}

void dialog_window::open_dialog() {
    {
        std::lock_guard<std::recursive_mutex> guard(m_mutex);
        if (m_opened) return;
    }
    m_opened = true;
    std::thread(&dialog_window::open, this).detach();
}

void dialog_window::open() {

    static char chPath[1024] = { 0 };
    bool selected = false;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
                                      COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            hr = pFileOpen->SetDefaultExtension(L"mp4;mov,m4a,fmp4");
            // Show the Open dialog box.
            hr = pFileOpen->Show(g_hwnd);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
//                        MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        wcstombs(chPath, pszFilePath, 1024);
                        MM_LOG_INFO("open file %s", chPath);
                        selected = true;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    m_opened = false;
    if (selected) {
        m_selected = true;
        m_path = chPath;
    }
    MM_LOG_INFO("open dialog thread finished");
}
