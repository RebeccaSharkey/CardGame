// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Platform/Windows/Win32Window.h"
#include <iostream>

int main() {
    Win32Window window(1280, 720, "Card Game");

    while (!window.ShouldClose()) {
        window.PollEvents();
        // Here we'll render each frame
    }

    return 0;
}