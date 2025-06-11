// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Platform/Windows/Win32Window.h"
#include "Renderer/VulkanRenderer.h"

int main() {
    Win32Window window(1280, 720, "Card Game");

    VulkanRenderer renderer;
    if (!renderer.Init(window.GetWindowHandle(), "Card Game"))
    {
        return -1;
    }

    while (!window.ShouldClose()) {
        window.PollEvents();
        // Here we'll render each frame
    }

    renderer.Cleanup();
    return 0;
}
