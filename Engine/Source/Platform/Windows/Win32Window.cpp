// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Platform/Platform.h"
#include "Win32Window.h"

Win32Window::Win32Window()
    : m_WindowHandle(nullptr), m_WindowClosed(false)
{
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = platform_window_callback;
    windowClass.hInstance = GetModuleHandle(nullptr);
    windowClass.lpszClassName = "Platform_Window";
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&windowClass)) {
        MessageBox(nullptr, "Window could not be created!", "Error", MB_ICONEXCLAMATION | MB_OK);
        exit(-1); // or throw
    }
}

Win32Window::~Win32Window()
{
    if ( m_WindowHandle != nullptr )
    {
        DestroyWindow(m_WindowHandle);
    }
}

bool Win32Window::InitWindow(int width, int height, const char *title)
{
    m_WindowHandle = CreateWindowExA(
        WS_EX_APPWINDOW,
        "Platform_Window", title,
        WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        100, 100, width, height,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (m_WindowHandle == nullptr)
    {
        MessageBox(nullptr, "Failed to create window", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(m_WindowHandle, SW_SHOW);
    return true;
}

HWND Win32Window::GetWindowHandle() const
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

bool Win32Window::ShouldClose() const
{
    return m_WindowClosed;
}

LRESULT Win32Window::platform_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
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

    return DefWindowProc(window, message, wParam, lParam);
}

void platform_get_window_size(uint32_t *width, uint32_t *height, void* windowHandle)
{
    RECT rect;
    GetClientRect(HWND(windowHandle), &rect);

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}