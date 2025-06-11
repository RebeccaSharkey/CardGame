// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <Windows.h>

class Win32Window {

public:
    Win32Window(int width, int height, const char* title);
    ~Win32Window();

    [[nodiscard]] HWND GetWindowHandle() const;

    void PollEvents();
    [[nodiscard]] bool ShouldClose() const;

private:
    HWND m_WindowHandle;
    bool m_WindowClosed;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
