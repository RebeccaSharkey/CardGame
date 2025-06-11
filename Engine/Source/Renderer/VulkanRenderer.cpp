// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "VulkanRenderer.h"
#include <iostream>

bool VulkanRenderer::Init(HWND windowHandle, const char* applicationName)
{
    // Application Info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = "CAD Engine";
    appInfo.pApplicationName = applicationName;
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // Create Info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // No extensions yet (we'll fix this next step!)
    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
    {
        std::cerr << "Failed to create Vulkan instance" << std::endl;
        return false;
    }

    std::cout << "Vulkan instance created successfully" << std::endl;
    return true;
}

void VulkanRenderer::Cleanup()
{
    if (m_Instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
