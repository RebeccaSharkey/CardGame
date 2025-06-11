// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>
#include <Windows.h>

class VulkanRenderer
{

public:
    bool Init(HWND windowHandle, const char* applicationName);
    void Cleanup() const;

private:
    VkInstance m_Instance = VK_NULL_HANDLE;
};
