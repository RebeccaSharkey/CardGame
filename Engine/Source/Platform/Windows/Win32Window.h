// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <Windows.h>

class Win32Window {

public:
    Win32Window();
    ~Win32Window();

    bool InitWindow(int width, int height, const char* title);

    HWND GetWindowHandle() const;

    void PollEvents();
    bool ShouldClose() const;

private:
    HWND m_WindowHandle;
    bool m_WindowClosed;
    static LRESULT CALLBACK platform_window_callback(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
};
