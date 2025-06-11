// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "Platform/Windows/Win32Window.h"
#include "Renderer/VulkanRenderer.h"

int main() {

    Win32Window window{};
    if (!window.InitWindow(1600, 720, "Card Game"))
    {
        return -1;
    }

    VulkanRenderer renderer;
    if (!renderer.Init("Card Game", window.GetWindowHandle()))
    {
        return -1;
    }

    while (!window.ShouldClose())
    {
        window.PollEvents();
        renderer.Render();
    }

    renderer.Cleanup();
    return 0;
}
