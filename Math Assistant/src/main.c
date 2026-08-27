#include "pch.h"
#include "platform.h"

#include <main_wnd.h>
#include <winutil.h>

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    HANDLE hMutex = CreateMutex(NULL, TRUE, _T("MyUniqueAppMutex"));

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(NULL, _T("Another instance of this application is already running."), _T("Single Instance"), MB_ICONEXCLAMATION | MB_OK);
        return 0; // Exit the application
    }

#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    RedirectIOToConsole();
#endif

    if (!LoadLibrary(L"msftedit.dll"))
    {
        ShowError(L"Unable to load library msftedit!");
        return -1;
    }

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        ShowError(_T("WinMain:CoInitialize"));
        return -1;
    }

    if (!MainWindow_RegisterClass())
    {
        ShowError(_T("Call to RegisterClass failed!"));

        return -1;
    }

    MainWindow* mainWindow = MainWindow_init();

    if (!MainWindow_Create(mainWindow))
    {
        ShowError(_T("Call to CreateWindow failed!"));

        return -1;
    }

    GraphicsWindow_Initial3D(mainWindow->_pGraphicsWnd);

    ShowWindow(mainWindow->_hWnd, nCmdShow);
    UpdateWindow(mainWindow->_hWnd);

    GameTimer_Init(&mainWindow->_pGraphicsWnd->_gameTimer);
    GameTimer_Reset(&mainWindow->_pGraphicsWnd->_gameTimer);

    // Main message loop:
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            GameTimer_Tick(&mainWindow->_pGraphicsWnd->_gameTimer);

            if (!mainWindow->_pGraphicsWnd->_paused)
            {
                GraphicsWindow_Update(mainWindow->_pGraphicsWnd);
                GraphicsWindow_Draw(mainWindow->_pGraphicsWnd);
            }
            else
            {
                Sleep(100);
            }
        }
    }

    MainWindow_free(mainWindow);

    CoUninitialize();

    // Release the mutex when the application exits
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }

    return (int)msg.wParam;
}

