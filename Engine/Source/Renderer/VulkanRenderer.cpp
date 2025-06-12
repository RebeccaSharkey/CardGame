// © 2025 Ctrl Alt Delete Games. All rights reserved.

#include "defines.h"
#include "Platform/Platform.h"
#include "VulkanRenderer.h"

#if WIN32
#include "Platform/Windows/Win32Window.h"
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#else
// other OS
#endif

#define ArraySize(array) (sizeof(array) / sizeof(array[0]))

#define VK_CHECK_RESULT(result)             \
    if(result != VK_SUCCESS)                \
    {                                       \
        std::cerr << result << std::endl;   \
        __debugbreak();                     \
        return false;                       \
    }

bool VulkanRenderer::Init(const char* applicationName, void* windowHandle)
{
    platform_get_window_size(&m_VkContext.screenSize.width, &m_VkContext.screenSize.height, windowHandle);

    // Create Instance
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pEngineName = "CAD Engine";
        appInfo.pApplicationName = applicationName;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        const char *extensions[] = {
            #if WIN32
                VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            #endif
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_KHR_SURFACE_EXTENSION_NAME};

        const char *layers[] = { "VK_LAYER_KHRONOS_validation" };

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.ppEnabledExtensionNames = extensions;
        createInfo.enabledExtensionCount = std::size(extensions);
        createInfo.ppEnabledLayerNames = layers;
        createInfo.enabledLayerCount = std::size(layers);

        VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &m_VkContext.instance))
    }

    //Setup Debug
    {
        auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_VkContext.instance, "vkCreateDebugUtilsMessengerEXT"));

        if (vkCreateDebugUtilsMessengerEXT)
        {
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {};
            debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugMessengerCreateInfo.pfnUserCallback = vk_debug_callback;

            vkCreateDebugUtilsMessengerEXT(m_VkContext.instance, &debugMessengerCreateInfo, nullptr, &m_VkContext.debugMessenger);
        }
        else
        {
            std::cerr << "Failed to create debug utils messenger." << std::endl;
            return false;
        }
    }

    //Create Surface
    {
        #if WIN32


            VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            surfaceCreateInfo.hwnd = HWND(windowHandle);
            surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

            VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(m_VkContext.instance, &surfaceCreateInfo, nullptr, &m_VkContext.surface))
        #else

        #endif
    }

    // Get GPU and GPU Index
    {
        m_VkContext.graphicsFamilyIndex = UINT32_MAX;
        uint32_t gpuCount = 0;
        //TODO: Suballocation from Main Allocation
        VkPhysicalDevice gpus[10];
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_VkContext.instance, &gpuCount, nullptr));
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(m_VkContext.instance, &gpuCount, gpus));

        for (uint32_t i = 0; i < gpuCount; i++)
        {
            VkPhysicalDevice gpu = gpus[i];

            uint32_t queueFamilyCount = 0;
            //TODO: Suballocation from Main Allocation
            VkQueueFamilyProperties queueFamilyProperties[10] = {};

            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);
            vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilyProperties);

            for (uint32_t j = 0; j < queueFamilyCount; j++)
            {
                if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    VkBool32 presentSupport = false;
                    VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(gpu, j, m_VkContext.surface, &presentSupport));

                    if (presentSupport)
                    {
                        m_VkContext.graphicsFamilyIndex = j;
                        m_VkContext.gpu = gpu;
                        break;
                    }
                }
            }
        }

        if ( m_VkContext.graphicsFamilyIndex == UINT32_MAX )
        {
            return false;
        }
    }

    // Create Logical Device
    {
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_VkContext.graphicsFamilyIndex;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfo.queueCount = 1;

        const char *extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.ppEnabledExtensionNames = extensions;
        deviceCreateInfo.enabledExtensionCount = std::size(extensions);

        VK_CHECK_RESULT(vkCreateDevice(m_VkContext.gpu, &deviceCreateInfo,nullptr, &m_VkContext.device));

        vkGetDeviceQueue(m_VkContext.device, m_VkContext.graphicsFamilyIndex, 0, &m_VkContext.graphicsQueue);
    }

    // Create Swap Chain
    {
        uint32_t formatCount = 0;
        //TODO: Suballocation from Main Allocation
        VkSurfaceFormatKHR surfaceFormat[10];
        VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkContext.gpu, m_VkContext.surface, &formatCount, nullptr));
        VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(m_VkContext.gpu, m_VkContext.surface, &formatCount, surfaceFormat));

        for (uint32_t i = 0; i < formatCount; i++)
        {
            if (surfaceFormat[i].format == VK_FORMAT_B8G8R8A8_SRGB)
            {
                m_VkContext.surfaceFormat = surfaceFormat[i];
                break;
            }
        }

        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VkContext.gpu, m_VkContext.surface, &surfaceCapabilities));
        uint32_t imageCount = 0;
        imageCount = surfaceCapabilities.minImageCount + 1 > surfaceCapabilities.maxImageCount ? surfaceCapabilities.minImageCount + 1 : surfaceCapabilities.minImageCount;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.surface = m_VkContext.surface;
        swapchainCreateInfo.imageFormat = m_VkContext.surfaceFormat.format;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageArrayLayers = 1;

        VK_CHECK_RESULT(vkCreateSwapchainKHR(m_VkContext.device, &swapchainCreateInfo, nullptr, &m_VkContext.swapchain));

        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_VkContext.device, m_VkContext.swapchain, &m_VkContext.swapchainImageCount, nullptr));
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(m_VkContext.device, m_VkContext.swapchain, &m_VkContext.swapchainImageCount, m_VkContext.swapchainImages));

        // Create Image Views
        {
            VkImageViewCreateInfo imageViewCreateInfo = {};
            imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewCreateInfo.format = m_VkContext.surfaceFormat.format;
            imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewCreateInfo.subresourceRange.layerCount = 1;
            imageViewCreateInfo.subresourceRange.levelCount = 1;

            for (uint32_t i = 0; i < m_VkContext.swapchainImageCount; i++)
            {
                imageViewCreateInfo.image = m_VkContext.swapchainImages[i];
                VK_CHECK_RESULT(vkCreateImageView(m_VkContext.device, &imageViewCreateInfo, nullptr, &m_VkContext.swapchainImageViews[i]));
            }
        }
    }

    // Renderpass
    {
        VkAttachmentDescription colourAttachment = {};
        colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //Can endable multi-sampling here to blend colour
        colourAttachment.format = m_VkContext.surfaceFormat.format;

        VkAttachmentDescription attachments[] =
        {
            colourAttachment
        };

        VkAttachmentReference colourAttachmentReference = {};
        colourAttachmentReference.attachment = 0; //Index
        colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colourAttachmentReference;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.pAttachments = attachments;
        renderPassCreateInfo.attachmentCount = std::size(attachments);
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;


        VK_CHECK_RESULT(vkCreateRenderPass(m_VkContext.device, &renderPassCreateInfo, nullptr, &m_VkContext.renderPass));
    }

    // Framebuffers
    {
        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.height = m_VkContext.screenSize.height;
        framebufferCreateInfo.width = m_VkContext.screenSize.width;
        framebufferCreateInfo.renderPass = m_VkContext.renderPass;
        framebufferCreateInfo.layers = 1;
        framebufferCreateInfo.attachmentCount = 1;

        for (uint32_t i = 0; i < m_VkContext.swapchainImageCount; i++)
        {
            framebufferCreateInfo.pAttachments = &m_VkContext.swapchainImageViews[i];
            VK_CHECK_RESULT(vkCreateFramebuffer(m_VkContext.device, &framebufferCreateInfo, nullptr, &m_VkContext.framebuffers[i]));
        }
    }

    // Create Command Pool
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo = {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.queueFamilyIndex = m_VkContext.graphicsFamilyIndex;

        VK_CHECK_RESULT(vkCreateCommandPool(m_VkContext.device, &commandPoolCreateInfo, nullptr, &m_VkContext.commandPool));
    }

    // Sync Objects
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VK_CHECK_RESULT(vkCreateSemaphore(m_VkContext.device, &semaphoreCreateInfo, nullptr, &m_VkContext.submitSemaphore));
        VK_CHECK_RESULT(vkCreateSemaphore(m_VkContext.device, &semaphoreCreateInfo, nullptr, &m_VkContext.acquireSemaphore));
    }

    return true;
}

bool VulkanRenderer::Render() const
{
    uint32_t imageIndex = 0;
    VK_CHECK_RESULT(vkAcquireNextImageKHR(m_VkContext.device, m_VkContext.swapchain, 0, m_VkContext.acquireSemaphore, nullptr, &imageIndex));

    // Allocate Command Buffer
    VkCommandBuffer cmd = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo cmdAllocateInfo = {};
    cmdAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocateInfo.commandBufferCount = 1;
    cmdAllocateInfo.commandPool = m_VkContext.commandPool;
    VK_CHECK_RESULT(vkAllocateCommandBuffers(m_VkContext.device, &cmdAllocateInfo, &cmd));

    // Start Command Buffer
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK_RESULT(vkBeginCommandBuffer(cmd, &beginInfo))

    VkClearValue clearValue = {};
    clearValue.color = {0.015f, 0.015f, 0.02f, 1.0f};

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_VkContext.renderPass;
    renderPassInfo.renderArea.extent = m_VkContext.screenSize;
    renderPassInfo.framebuffer = m_VkContext.framebuffers[imageIndex];
    renderPassInfo.pClearValues = &clearValue;
    renderPassInfo.clearValueCount = 1;

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Rendering Commands
    {

    }

    vkCmdEndRenderPass(cmd);

    VK_CHECK_RESULT(vkEndCommandBuffer(cmd))

    // Submit Graphics Queue
    constexpr VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.pSignalSemaphores = &m_VkContext.submitSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_VkContext.acquireSemaphore;
    submitInfo.waitSemaphoreCount = 1;
    VK_CHECK_RESULT(vkQueueSubmit(m_VkContext.graphicsQueue, 1, &submitInfo, nullptr));

    // Present Graphics Queue
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pSwapchains = &m_VkContext.swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pWaitSemaphores = &m_VkContext.submitSemaphore;
    presentInfo.waitSemaphoreCount = 1;
    VK_CHECK_RESULT(vkQueuePresentKHR(m_VkContext.graphicsQueue, &presentInfo));

    // Wait for Idle Command Buffer and then Free Up the Command Buffer
    // TODO: Remove wait for idle
    VK_CHECK_RESULT(vkDeviceWaitIdle(m_VkContext.device));
    vkFreeCommandBuffers(m_VkContext.device, m_VkContext.commandPool, 1, &cmd);

    return true;
}

void VulkanRenderer::Cleanup() const
{
    if (m_VkContext.device != VK_NULL_HANDLE) {
        if (m_VkContext.swapchain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(m_VkContext.device, m_VkContext.swapchain, nullptr);
        }
        vkDestroyDevice(m_VkContext.device, nullptr);
    }

    if (m_VkContext.debugMessenger != VK_NULL_HANDLE) {
        const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_VkContext.instance, "vkDestroyDebugUtilsMessengerEXT"));

        if (vkDestroyDebugUtilsMessengerEXT) {
            vkDestroyDebugUtilsMessengerEXT(m_VkContext.instance, m_VkContext.debugMessenger, nullptr);
        }
    }

    if (m_VkContext.surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_VkContext.instance, m_VkContext.surface, nullptr);
    }

    if (m_VkContext.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_VkContext.instance, nullptr);
    }
}
