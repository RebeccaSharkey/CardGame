// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Win32Window.h"

Win32Window::Win32Window(int width, int height, const char* title)
    : m_WindowClosed(false)
{
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.lpszClassName = "CardGameWindow";

    RegisterClass(&windowClass);

    m_WindowHandle = CreateWindowEx(0, windowClass.lpszClassName, title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, windowClass.hInstance, nullptr);

    SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

Win32Window::~Win32Window()
{
    DestroyWindow(m_WindowHandle);
}

HWND Win32Window::GetWindowHandle()
{
    return m_WindowHandle;
}

void Win32Window::PollEvents()
{
    MSG message;

    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            m_WindowClosed = true;
        }
    }
}

bool Win32Window::ShouldClose()
{
    return m_WindowClosed;
}

LRESULT Win32Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_DESTROY:
            return 0;

        default:
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
