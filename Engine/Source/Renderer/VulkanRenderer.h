// © 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cout << "Validation Error: " << pCallbackData->pMessage << std::endl;
    return false;
}

struct VkContext
{
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

    VkInstance instance = VK_NULL_HANDLE;

    VkExtent2D screenSize = {};

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSurfaceFormatKHR surfaceFormat = {};

    VkPhysicalDevice gpu = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkRenderPass renderPass = VK_NULL_HANDLE;

    VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
    VkSemaphore submitSemaphore = VK_NULL_HANDLE;

    uint32_t swapchainImageCount = {};
    //TODO: Suballocation from Main Allocation
    VkImage swapchainImages[5] = {};

    VkImageView swapchainImageViews[5] = {};
    VkFramebuffer framebuffers[5] = {};

    uint32_t graphicsFamilyIndex = {};
};

class VulkanRenderer
{

public:
    bool Init(const char* applicationName, void*  windowHandle);

    bool Render() const;

    void Cleanup() const;



private:
    VkContext m_VkContext = {};
};
