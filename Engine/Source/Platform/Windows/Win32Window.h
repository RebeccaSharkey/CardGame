// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <Windows.h>

class Win32Window {

public:
    Win32Window(int width, int height, const char* title);
    ~Win32Window();

    HWND GetWindowHandle();

    void PollEvents();
    bool ShouldClose();

private:
    HWND m_WindowHandle;
    bool m_WindowClosed;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
